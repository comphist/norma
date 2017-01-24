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
Cycle::~Cycle() {
    if (_data != nullptr)
        delete _data;
    if (_applicator != nullptr)
        delete _applicator;
}

void Cycle::init(Input *input, Output* output,
                 const std::map<std::string, std::string>& params) {
    _in = input;
    _out = output;
    _params = params;
    _data = new TrainingData();
    _in->initialize(this, _out, _data);
    _out->initialize(this, _in, _data);
    set_thread(_in->thread_suitable() && _out->thread_suitable());
}

void Cycle::init_chain(const std::string& chain_definition,
                       const std::string& plugin_base) {
    if (_in == nullptr || _out == nullptr)
        throw std::runtime_error("Cycle was not intialized!");
    _applicator = new Applicator(chain_definition, plugin_base, _params);
}

bool Cycle::output_thread() {
    do {
        std::unique_lock<std::mutex> output_lock(output_mutex);
        output_condition.wait(output_lock,
                              [this]{ return output_ready; });
        while (!results.empty()) {
            Normalizer::Result r = results.front().get();
            _out->put_line(&r, settings["prob"], _max_log_level);
            results.pop();
        }
        output_ready = false;
    } while (!workers_done);
    return true;
}

Normalizer::Result Cycle::worker_thread(const string_impl& line) {
    return _applicator->normalize(line);
}

void Cycle::start() {
    _in->begin();
    output_done = std::async(policy, &Cycle::output_thread, this);
    workers_done = false;
    output_ready = false;
    while (!_in->request_quit()) {
        string_impl line = _in->get_line();
        if (line.length() == 0)
            continue;
        if (settings["train"] && _in->request_train()) {
            training_pair(line);
            continue;
        }
        if (settings["normalize"]) {
            results.push(std::async(policy,
                                    &Cycle::worker_thread,
                                    this, line));
            if (!output_ready) {
                output_ready = true;
                output_condition.notify_all();
            }
        }
        if (settings["train"] && _out->request_train())
            _applicator->train(_data);
    }
    // if output isn't ready by now, we haven't normalized at all
    if (!output_ready) {
        output_ready = true;
        output_condition.notify_all();
    }
    workers_done = true;
    output_done.get();
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
        _applicator->train(_data);
        return true;
    }
    return false;
}

void Cycle::save_params() {
    _applicator->save_params();
}
}  // namespace Norma

