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
#ifndef INTERFACE_IOBASE_H_
#define INTERFACE_IOBASE_H_
#include<vector>
#include<functional>
#include"string_impl.h"
#include"training_data.h"

namespace Norma {
class Cycle;

/// Base class for both Input and Output.
/** Provides all methods that are common to both classes,
 *  such as accessing the history.
 *
 *  I'd like to have this pure virtual, but can't think
 *  of anything to sensibly set to 0.
 **/
class IOBase {
 public:
     IOBase() = default;
     virtual ~IOBase() = default;
     virtual TrainingData& training_data() {
         return *_training;
     }
     void initialize(Cycle* c, IOBase* opp, TrainingData* data) {
         _cycle = c;
         _opposite = opp;
         _training = data;
     }
     void store_last() {
         store_line(_line);
     }
     virtual bool thread_suitable() {
         return false;
     }
 protected:
     virtual void store_line(const string_impl& line) = 0;
     TrainingData* _training;
     Cycle* _cycle;
     IOBase* _opposite;
     string_impl _line;
};
}  // namespace Norma
#endif  // INTERFACE_IOBASE_H_

