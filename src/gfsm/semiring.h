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
#ifndef GFSM_SEMIRING_H_
#define GFSM_SEMIRING_H_
#include"gfsmlibs.h"

namespace Gfsm {

/// Identify a semiring by type.
enum class SemiringType {
    UNKNOWN = gfsmSRTUnknown,
    BOOLEAN = gfsmSRTBoolean,
    LOG     = gfsmSRTLog,
    REAL    = gfsmSRTReal,
    TRIVIAL = gfsmSRTTrivial,
    TROPICAL = gfsmSRTTropical,
    PLOG    = gfsmSRTPLog,
    ARCTIC  = gfsmSRTArctic,
    FUZZY   = gfsmSRTFuzzy,
    PROB    = gfsmSRTProb
};

}  // namespace Gfsm

#endif  // GFSM_SEMIRING_H_

