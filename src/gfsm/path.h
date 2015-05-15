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
#ifndef GFSM_PATH_H_
#define GFSM_PATH_H_
#include<tuple>
#include<vector>
#include"alphabet.h"
#include"labelvector.h"
#include"string_impl.h"

namespace Gfsm {

class StringPath;

/// A weight.ed path in a finite-state automaton.
class Path {
     friend class StringPath;
 public:
     Path() = default;
     Path(LabelVector i, LabelVector o, double w)
         : input(i), output(o), weight(w) {}

     bool operator==(const Path& that) const {
         return (input == that.input &&
                 output == that.output &&
                 weight == that.weight);
     }
     inline bool operator!=(const Path& that) const {
         return !(*this == that);
     }
     bool operator<(const Path& that) const {
         return (std::make_tuple(input, output, weight)
                 < std::make_tuple(that.input, that.output, that.weight));
     }
     const LabelVector& get_output() const { return output; }
     const LabelVector& get_input() const { return input; }
     const double& get_weight() const { return weight; }

 protected:
     LabelVector input;  /**< Input label sequence.  */
     LabelVector output; /**< Output label sequence. */
     double      weight; /**< Weight of the path.    */
};

/// A weighted path in a string-based finite-state automaton.
/** Also stores symbol sequences in addition to label sequences.
 */
class StringPath : public Path {
 public:
     StringPath(std::vector<string_impl> i, std::vector<string_impl> o,
                double w)
         : input(i), output(o) { weight = w; }

     static StringPath from(const Path& p, const Alphabet& alph_in,
                            const Alphabet& alph_out) {
         return StringPath(alph_in.map_labels_to_vector(p.input),
                           alph_out.map_labels_to_vector(p.output),
                           p.weight);
     }
     const std::vector<string_impl>& get_output() const {
         return output;
     }
     const std::vector<string_impl>& get_input() const {
         return input;
     }
 private:
     std::vector<string_impl> input;  /**< Input symbol sequence.  */
     std::vector<string_impl> output; /**< Output symbol sequence. */
};

}  // namespace Gfsm

#endif  // GFSM_PATH_H_

