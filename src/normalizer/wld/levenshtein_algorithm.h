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
#ifndef NORMALIZER_WLD_LEVENSHTEIN_ALGORITHM_H_
#define NORMALIZER_WLD_LEVENSHTEIN_ALGORITHM_H_
#include<vector>
#include"string_impl.h"
#include"typedefs.h"
#include"weight_set.h"

namespace Norma {
namespace Normalizer {
namespace WLD {

AlignmentSet align(const std::vector<string_impl>& source,
                   const std::vector<string_impl>& target,
                   const WeightSet& weights);
AlignmentSet align(const string_impl& from, const string_impl& to,
                   const WeightSet& weights);
AlignmentSet align(const WordPair& p, const WeightSet& weights);

double wld(const std::vector<string_impl>& source,
           const std::vector<string_impl>& target,
           const WeightSet& weights);
double wld(const string_impl& from, const string_impl& to,
           const WeightSet& weights);
double wld(const WordPair& p, const WeightSet& weights);
double wld(const EditPair& p, const WeightSet& weights);

}  // namespace WLD
}  // namespace Normalizer
}  // namespace Norma

#endif  // NORMALIZER_WLD_LEVENSHTEIN_ALGORITHM_H_

