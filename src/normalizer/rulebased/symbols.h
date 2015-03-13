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
#ifndef NORMALIZER_RULEBASED_SYMBOLS_H_
#define NORMALIZER_RULEBASED_SYMBOLS_H_
#include"string_impl.h"

namespace Norma {
namespace Normalizer {
namespace Rulebased {

/// Magic symbols for rule-based normalization
// --- maybe refactor this at some point
namespace Symbols {
    const string_impl EPSILON  = "E";
    const char_impl BOUNDARY = '#';
}

}  // namespace Rulebased
}  // namespace Normalizer
}  // namespace Norma

#endif  // NORMALIZER_RULEBASED_SYMBOLS_H_
