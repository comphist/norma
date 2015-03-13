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
#ifndef NORMALIZER_WLD_TYPEDEFS_H_
#define NORMALIZER_WLD_TYPEDEFS_H_
#include<map>
#include<utility>
#include<vector>
#include"string_impl.h"

namespace Norma {
namespace Normalizer {
namespace WLD {
typedef std::pair<string_impl, string_impl> WordPair;
typedef std::map<WordPair, int> TrainSet;

typedef std::pair<std::vector<string_impl>, std::vector<string_impl>> EditPair;
typedef std::vector<EditPair> RuleSet;
typedef std::vector<RuleSet> AlignmentSet;

typedef std::vector<std::vector<double>> DistanceMatrix;
}  // namespace WLD
}  // namespace Normalizer
}  // namespace Norma

#endif  // NORMALIZER_WLD_TYPEDEFS_H_
