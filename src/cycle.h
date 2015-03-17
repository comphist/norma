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
#include<vector>
#include<string>
#include<fstream>
#include<stdexcept>
#include<functional>
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
     Cycle(Input* in, Output* out,
           const std::string& chain_definition,
           const std::map<std::string, std::string>& params);
     Cycle() = delete;
     Cycle(const Cycle& c) = delete;
     Cycle& operator=(const Cycle& c) = delete;
     ~Cycle();
     /// start processing data
     void start();

     /// save all normalizer parameter files
     void save_params();
     /// turn printing probabilities on/off
     inline void do_print_prob(bool status) {
         _prob = status;
     }
     inline bool print_prob() {
         return _prob;
     }
     /// set training on/off
     inline void do_train(bool status) {
         _train = status;
     }
     inline bool train() {
         return _train;
     }
     /// set normalizing on/off
     inline void do_norm(bool status) {
         _norm = status;
     }
     inline bool norm() {
         return _norm;
     }
     void set_thread(bool status) {
         _thread = status;
     }

 private:
     /// check input for training pair and add them to
     /// in/out history if appropriate
     bool training_pair(const string_impl& line);
     void each_applicator(std::function<void(Applicator*)> fun);
     bool _train = true,
          _norm  = true,
          _prob  = true,
          _thread = false;
     Normalizer::LogLevel _max_log_level = Normalizer::LogLevel::WARN;
     TrainingData* _data;
     Input*  _in;
     Output* _out;
     std::vector<Applicator*> _applicators;
};
}  // namespace Norma

#endif  // CYCLE_H_

