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
#include"cycle.h"
#include<map>
#include<vector>
#include<string>
#include<fstream>
#include<functional>
#include<stdexcept>
#include<thread>
#include<future>
#include<cctype>
#include"normalizer/result.h"
#include"interface.h"
#include"applicator.h"
#include"training_data.h"

using std::map;
using std::string;

namespace Norma {
Cycle::Cycle(Input *in, Output *out,
             const string& chain_definition,
             const map<string, string>& params)
    : _in(in), _out(out) {
    _applicators.push_back(new Applicator(chain_definition, params));
    _data = new TrainingData();
    for (auto app : _applicators)
        app->init_chain();
    _in->initialize(this, _out, _data);
    _out->initialize(this, _in, _data);
    _thread = _in->thread_suitable() && _out->thread_suitable();
}

Cycle::~Cycle() {
    delete _data;
    for (auto app_ptr : _applicators)
        delete app_ptr;
}

void Cycle::each_applicator(std::function<void(Applicator*)> fun) {
    for (auto app : _applicators)
        fun(app);
}

void Cycle::start() {
    _in->begin();
    auto* future_results = new std::vector<std::future<Normalizer::Result>>();
    while (!_in->request_quit()) {
        string_impl line = _in->get_line();
        if (line.length() == 0)
            continue;
        if (_train && training_pair(line))
            continue;
        Normalizer::Result result;
        if (_norm)  // XXX multiple applicators here
            if (_thread) {
                // threaded normalization
                std::packaged_task<Normalizer::Result()> norm([=]() {
                    return _applicators.front()->normalize(line);
                });
                future_results->push_back(norm.get_future());
                std::thread(std::move(norm)).detach();
            } else {
                result = _applicators.front()->normalize(line);
            }
        else  // NOLINT[whitespace/newline]
            result = Normalizer::Result(line, 0.0);
        if (!_thread)
            _out->put_line(&result, _prob);
        if (_train)
            each_applicator([&](Applicator* app) {
                app->train(_data);
            });
    }
    if (_thread) {
        for (auto& fr : *future_results) {
            Normalizer::Result result = fr.get();
            _out->put_line(&result, _prob);
        }
        delete future_results;
    } else {
        delete future_results;
    }
    _in->end();
}

bool Cycle::training_pair(const string_impl& line) {
    string_size divpos = 0;
    for (string_size i = 1; i < line.length(); ++i)
        if (isspace(line[i])
         && i != line.length() - 1
         && !isspace(line[i+1]))
            divpos = i;
    if (divpos != 0) {
        string_impl word,
                    modern;
        extract(line, 0, divpos, &word);
        extract(line, divpos + 1, line.length(), &modern);
        _data->add_source(word);
        _data->add_target(modern);
        each_applicator([&](Applicator* app) {
            app->train(_data);
        });
        return true;
    }
    return false;
}

void Cycle::save_params() {
    for (auto app : _applicators)
        app->save_params();
}
}  // namespace Norma

