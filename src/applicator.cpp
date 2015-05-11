/* Copyright 2013-2015 Marcel Bollmann, Florian Petran
 *
 * This file is part of Norma.
 *
 * Norma is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * Norma is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with Norma.  If not, see <http://www.gnu.org/licenses/>.
 */
#include"applicator.h"
#include<dlfcn.h>  // linux specific XXX
#include<string>
#include<stdexcept>
#include<cctype>
#include<map>
#include<list>
#include<utility>
#include<algorithm>
#include<future>
#include<thread>
#include<chrono>
#include<iostream>
#include"training_data.h"
#include"normalizer/exceptions.h"
#include"lexicon/lexicon.h"
#include"normalizer/base.h"

using std::map;
using std::string;

namespace Norma {

Applicator::Applicator(const string& chain_definition,
                       const std::string& plugin_base,
                       const map<string, string>& params)
    : config_vars(params) {
    chain_def = chain_definition;
    _lex = new Normalizer::Lexicon();
    try {
        _lex->init(params);
    } catch(Normalizer::init_error e) {
        std::cerr << "*** WARNING: while initializing Lexicon: "
                  << e.what() << std::endl;
    }
}

Applicator::~Applicator() {
    delete _lex;
    for (auto normalizer : created_normalizers)
        normalizer.first(normalizer.second);
    for (auto plugin : loaded_plugins)
        dlclose(plugin);
}

void Applicator::push_chain(Normalizer::Base* n) {
    push_back(n);
}

void Applicator::init_chain() {
    const std::string& nlist = chain_def;
    size_t left = 0,
           right = nlist.find(",");
    do {
        size_t len = (right != std::string::npos)
                     ? right - left : std::string::npos;
        Normalizer::Base* normalizer = nullptr;
        std::string norm_name = nlist.substr(left, len);
        // strip whitespace
        norm_name.erase(std::remove_if(norm_name.begin(), norm_name.end(),
                                       isspace), norm_name.end());
        try {
            normalizer = create_plugin(norm_name);
            normalizer->init(config_vars, _lex);
        } catch(Normalizer::init_error e) {
            std::cerr << "*** WARNING: while initializing normalizer "
                      << norm_name << ": "
                      << e.what() << std::endl;
        } catch (std::runtime_error e) {
            std::cerr << "*** ERROR: while loading plugin "
                      << norm_name << ": "
                      << e.what() << std::endl;
        }
        if (normalizer != nullptr)
            push_chain(normalizer);
        left = right;
        right = nlist.find(",", right + 1);
    } while (left++ != std::string::npos);
}

Normalizer::Result Applicator::normalize(const string_impl& word) const {
    std::list<std::future<Normalizer::Result>> results;
    unsigned int priority = 1;
    for (auto normalizer : *this) {
        std::packaged_task<Normalizer::Result()> task([=]() {
                Normalizer::Result result = (*normalizer)(word);
                result.priority = priority;
                return result;
        });
        results.push_back(task.get_future());
        std::thread(std::move(task)).detach();
        ++priority;
    }

    Normalizer::Result bestresult(word, 0);
    for (std::future<Normalizer::Result>& fr : results) {
        Normalizer::Result my_result = fr.get();
        bestresult = chooser(bestresult, my_result);
    }
    return bestresult;
}

void Applicator::train(TrainingData *data) {
    if (data->empty())
        return;

    // update the lexicon
    _lex->add(data->rbegin()->target());

    std::list<std::future<bool>> train_done;
    // start the training threads and detach
    for (auto normalizer : *this) {
        std::packaged_task<bool()> train_task([=]() {
            std::lock_guard<std::mutex> train_guard(normalizer->mutex);
            bool result = normalizer->train(data);
            return result;
        });
        train_done.push_back(train_task.get_future());
        std::thread(std::move(train_task)).detach();
    }

    // keep querying the futures to see if training is
    // finished
    auto pos = train_done.begin();
    std::future_status status;
    while (!train_done.empty()) {
        status = pos->wait_for(std::chrono::milliseconds(5));
        if (status == std::future_status::ready)
            pos = train_done.erase(pos);
        else
            ++pos;
        if (pos == train_done.end())
            pos = train_done.begin();
    }

    for (auto pp = data->rbegin(); pp != data->rend(); ++pp) {
        if (pp->is_used())
            break;
        pp->make_used();
    }
}

const Normalizer::Result&
    Applicator::best_score(const Normalizer::Result& one,
                           const Normalizer::Result& two) {
    return (one.score > two.score) ? one : two;
}

const Normalizer::Result&
    Applicator::best_priority(const Normalizer::Result& one,
                              const Normalizer::Result& two) {
    if ((one.priority < two.priority) && one.score > 0.0)
        return one;
    else
        return two;
}

void Applicator::save_params() {
    for (auto n : *this) {
        try {
            n->save_params();
        } catch(std::runtime_error e) {
            std::cerr << "*** ERROR: while saving params for normalizer"
                      << n->name() << ":" << std::endl
                      << e.what() << std::endl;
        }
    }
    try {
        _lex->save_params();
    } catch (std::runtime_error e) {
        std::cerr << "*** ERROR: while saving params for Lexicon:"
                  << std::endl << e.what() << std::endl;
    }
}

Normalizer::Base* Applicator::create_plugin(const std::string& name) {
    // this is linux specific now
    std::string plugin_name = plugin_base + "/lib" + name + ".so";
    void* plugin = dlopen(plugin_name.c_str(), RTLD_LAZY);
    if (!plugin)
        throw std::runtime_error("Normalizer plugin not found: "
                                 + plugin_name);
    dlerror();
    create_t* create_plugin = (create_t*) dlsym(plugin, "create_normalizer");
    const char* dlsym_error = dlerror();
    if (dlsym_error)
        throw std::runtime_error("Error loading symbol 'create_normalizer': "
                                 + std::string(dlsym_error));
    destroy_t* destroy_plugin
        = (destroy_t*) dlsym(plugin, "destroy_normalizer");
    dlsym_error = dlerror();
    if (dlsym_error)
        throw std::runtime_error("Error loading symbol 'destroy_normalizer': "
                                 + std::string(dlsym_error));
    Normalizer::Base* normalizer = create_plugin();
    created_normalizers.push_back(std::make_pair(destroy_plugin, normalizer));
    return normalizer;
}

}  // namespace Norma
