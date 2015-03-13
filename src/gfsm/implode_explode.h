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
#ifndef GFSM_IMPLODE_EXPLODE_H_
#define GFSM_IMPLODE_EXPLODE_H_
#include<vector>
#include"string_impl.h"

namespace Gfsm {

/// Convert symbol sequence from a string to a vector.
/** @param str Symbol sequence to convert
    @param att If true, characters enclosed in square brackets will be treated
               as a single symbol; if false, each character will be converted
               to a separate symbol.
 */
std::vector<string_impl> explode(const string_impl& str, bool att = false);

/// Convert symbol sequence from a vector to a string.
/** @param vec Symbol sequence to convert
    @param att If true, multi-character symbols will be enclosed in square
               brackets in the returned string.
 */
string_impl implode(const std::vector<string_impl>& vec, bool att = false);

}  // namespace Gfsm

#endif  // GFSM_IMPLODE_EXPLODE_H_
