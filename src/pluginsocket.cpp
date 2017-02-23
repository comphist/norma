/* Copyright 2013-2017 Marcel Bollmann, Florian Petran
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
#include"pluginsocket.h"
#include<dlfcn.h>  // linux specific XXX
#include<string>
#include<stdexcept>
#include<cctype>
#include<map>
#include<list>
#include<set>
#include<utility>
#include<algorithm>
#include<future>
#include<thread>
#include<chrono>
#include<iostream>
#include<sstream>
#include"training_data.h"
#include"normalizer/exceptions.h"
#include"lexicon/lexicon.h"
#include"normalizer/base.h"

using std::map;
using std::string;

namespace Norma {

PluginSocket::PluginSocket(const string& chain_definition,
                       const string& plugin_base_param,
                       const map<string, string>& params)
    : config_vars(params), chain_def(chain_definition),
      plugin_base(plugin_base_param) {
    _lex = new Normalizer::Lexicon();
    try {
        _lex->init(params);
    } catch(Normalizer::init_error e) {
        std::cerr << "*** WARNING: while initializing Lexicon: "
                  << e.what() << std::endl;
    }
    try {
        init_chain();
    } catch(Normalizer::init_error e) {
        std::cerr << "*** WARNING: while initializing a normalizer: "
                  << e.what() << std::endl;
    }
}

PluginSocket::~PluginSocket() {
    delete _lex;
    for (auto normalizer : created_normalizers)
        normalizer.first(normalizer.second);
    for (auto plugin : loaded_plugins)
        dlclose(plugin);
}

void PluginSocket::push_chain(Normalizer::Base* n) {
    push_back(n);
}

void PluginSocket::init_chain() {
    std::set<std::string> aliases;
    std::istringstream chain_stream(chain_def);
    string element;
    while (std::getline(chain_stream, element, ',')) {
        std::istringstream element_stream(element);
        string lib_name, alias;
        std::getline(element_stream, lib_name, ':');
        std::getline(element_stream, alias);
        auto trim = [](string* str) {
            str->erase(std::remove_if(str->begin(), str->end(),
                                     isspace), str->end());
        };
        trim(&lib_name);
        trim(&alias);
        alias = alias != "" ? alias : lib_name;
        if (aliases.count(alias) != 0) {
            std::cerr << "*** ERROR: alias '" << alias
                      << "' was already used! "
                      << "Skipping this instance of '"
                      << lib_name << "'" << std::endl;
            continue;
        } else {
            aliases.insert(alias);
        }

        Normalizer::Base* normalizer = nullptr;
        try {
            normalizer = create_plugin(lib_name, alias);
            normalizer->init(config_vars, _lex);
        } catch (Normalizer::init_error e) {
            std::cerr << "*** WARNING: while initializing normalizer "
                      << lib_name << ": "
                      << e.what() << std::endl;
        } catch (std::runtime_error e) {
            std::cerr << "*** ERROR: while loading plugin "
                      << lib_name << ": "
                      << e.what() << std::endl;
        }
        if (normalizer != nullptr)
            push_chain(normalizer);
    }
}

Normalizer::Result PluginSocket::normalize(const string_impl& word) const {
    // starting all normalizers async seems to make everything slower
    // presumably this is because the cascade_lookup blocks
    unsigned int priority = 1;
    Normalizer::Result result, bestresult(word, 0);
    for (auto normalizer : *this) {
        result = (*normalizer)(word);
        result.priority = priority;
        bestresult = chooser(&bestresult, &result);
        if (bestresult.is_final)
            break;
        ++priority;
    }
    return bestresult;
}

void PluginSocket::train(TrainingData *data) {
    if (data->empty())
        return;

    // update the lexicon
    _lex->add(data->rbegin()->target());

    std::list<std::future<bool>> train_done;
    // start the training threads and detach
    for (auto normalizer : *this) {
        std::packaged_task<bool()> train_task([=]() {
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
    PluginSocket::best_score(Normalizer::Result* one,
                           Normalizer::Result* two) {
    return (one->score > two->score) ? *one : *two;
}

const Normalizer::Result&
    PluginSocket::best_priority(Normalizer::Result* one,
                              Normalizer::Result* two) {
    if ((two->priority < one->priority) && two->score > 0.0) {
        two->is_final = true;
        return *two;
    }
    return *one;
}

void PluginSocket::save_params() {
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

Normalizer::Base* PluginSocket::create_plugin(const std::string& lib_name,
                                            const std::string& alias) {
    // this is linux specific now
    std::string plugin_name = plugin_base + "/lib" + lib_name + ".so";
    std::string my_alias = alias == "" ? lib_name : alias;
    void* plugin = dlopen(plugin_name.c_str(), RTLD_LAZY);
    if (!plugin)
        throw std::runtime_error("Normalizer plugin not found: "
                                 + plugin_name);
    dlerror();
    create_t* create_plugin =
        reinterpret_cast<create_t*>(dlsym(plugin, "create_normalizer"));
    const char* dlsym_error = dlerror();
    if (dlsym_error)
        throw std::runtime_error("Error loading symbol 'create_normalizer': "
                                 + std::string(dlsym_error));
    destroy_t* destroy_plugin =
        reinterpret_cast<destroy_t*>(dlsym(plugin, "destroy_normalizer"));
    dlsym_error = dlerror();
    if (dlsym_error)
        throw std::runtime_error("Error loading symbol 'destroy_normalizer': "
                                 + std::string(dlsym_error));
    Normalizer::Base* normalizer = create_plugin();
    normalizer->set_name(my_alias);
    created_normalizers.push_back(std::make_pair(destroy_plugin, normalizer));
    return normalizer;
}

}  // namespace Norma

