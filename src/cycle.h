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
#ifndef CYCLE_H_
#define CYCLE_H_
#include<map>
#include<queue>
#include<string>
#include<future>
#include<condition_variable>
#include"string_impl.h"
#include"training_data.h"
#include"normalizer/result.h"

namespace Norma {
class Input;
class Output;
class Applicator;

/// the main application cycle
/** This class contains the main loop for input and output.
 *  It connects Input, Output, and Applicator classes.
 *  At some point it should support multiple Applicator instances
 *  and supply a way of choosing between their input, but right
 *  now, it's just the one.
 *
 *  Usually, an application should need just one Cycle object,
 *  but it's conceivable to have multiple ones, e.g. when
 *  normalizing several files parallel.
 **/

class Cycle {
 public:
     Cycle() = default;
     ~Cycle();
     //Cycle(const Cycle& c);
     //Cycle& operator=(const Cycle& c);
     void init(Input* input, Output* output,
               const std::map<std::string, std::string>& params);
     void init_chain(const std::string& chain_definition,
                     const std::string& plugin_base);

     void start();
     void save_params();
     void set(const std::string setting, bool val) {
         settings[setting] = val;
     }
     bool get(const std::string setting) const {
         return settings.at(setting);
     }
     void set_thread(bool val) {
         policy = val ? std::launch::async : std::launch::deferred;
     }

 private:
     bool training_pair(const string_impl& line);

     std::map<std::string, std::string> _params;
     Normalizer::LogLevel _max_log_level = Normalizer::LogLevel::WARN;
     std::map<std::string, bool> settings = {
         { "train", true },
         { "normalize", true },
         { "prob", true } };
     TrainingData* _data = nullptr;
     Applicator* _applicator = nullptr;
     Input* _in = nullptr;
     Output* _out = nullptr;

     bool output_thread();
     Normalizer::Result worker_thread(const string_impl& line);

     std::launch policy = std::launch::async|std::launch::deferred;
     std::queue<std::future<Normalizer::Result>> results;
     std::future<bool> output_done;
     std::mutex output_mutex;
     std::condition_variable output_condition;
     bool output_ready = false,
          workers_done = false;
};
}  // namespace Norma
#endif  // CYCLE_H_

