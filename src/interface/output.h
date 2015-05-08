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
#ifndef INTERFACE_OUTPUT_H_
#define INTERFACE_OUTPUT_H_
#include<string>
#include<iosfwd>
#include"iobase.h"
#include"normalizer/result.h"

namespace Norma {
/// Basic Output class. Not pure virtual since it's essentially
/// non-interactive output already.
class Output : public IOBase {
 public:
     Output();
     virtual ~Output() = default;
     /// put a line on the output device and record it in the history
     virtual void put_line(Normalizer::Result* result,
                           bool print_prob);
     bool thread_suitable() {
         return true;
     }

 protected:
     virtual void store_line(const string_impl& line) {
         _training->add_target(line);
     }
     std::ostream *_output;
};

/// Interactive output class that asks the user for validation of
/// each normalization.
class InteractiveOutput : public Output {
 public:
     void put_line(Normalizer::Result* result,
                   bool print_prob);
     std::string validate(const string_impl& line);
     bool thread_suitable() {
         return false;
     }

 private:
     const char *validate_prompt = "? ";
};
}  // namespace Norma
#endif  // INTERFACE_OUTPUT_H_

