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
#include"gfsm/builder.h"
#include"gfsm/automaton.h"
#include"gfsm/acceptor.h"
#include"gfsm/string_acceptor.h"
#include"gfsm/transducer.h"
#include"gfsm/string_transducer.h"
#include"gfsm/cascade.h"
#include"gfsm/string_cascade.h"
#include"gfsm/semiring.h"

namespace Gfsm {

Automaton AutomatonBuilder::make_automaton(SemiringType sr) {
    return Automaton(&gfsm_mutex, sr);
}

Acceptor AutomatonBuilder::make_acceptor(SemiringType sr) {
    return Acceptor(&gfsm_mutex);
}

StringAcceptor AutomatonBuilder::make_stringacceptor(SemiringType sr) {
    return StringAcceptor(&gfsm_mutex);
}

Transducer AutomatonBuilder::make_transducer(SemiringType sr) {
    return Transducer(&gfsm_mutex);
}

StringTransducer AutomatonBuilder::make_stringtransducer(SemiringType sr) {
    return StringTransducer(&gfsm_mutex);
}

Cascade AutomatonBuilder::make_cascade(unsigned int depth, SemiringType sr) {
    return Cascade(&gfsm_mutex, depth, sr);
}

StringCascade
AutomatonBuilder::make_stringcascade(unsigned int depth, SemiringType sr) {
    return StringCascade(&gfsm_mutex, depth, sr);
}
}  // namespace Gfsm

