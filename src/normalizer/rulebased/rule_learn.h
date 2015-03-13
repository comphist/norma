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
#ifndef NORMALIZER_RULEBASED_RULE_LEARN_H_
#define NORMALIZER_RULEBASED_RULE_LEARN_H_
#include"string_impl.h"
#include"rule.h"

namespace Norma {
namespace Normalizer {
namespace Rulebased {

/// the main function of this library
RuleSet learn_rules(const string_impl& source, const string_impl& target,
                    bool do_merge, bool insert_epsilon);

}  // namespace Rulebased
}  // namespace Normalizer
}  // namespace Norma
#endif  // NORMALIZER_RULEBASED_RULE_LEARN_H_

