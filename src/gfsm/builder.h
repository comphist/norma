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
#ifndef GFSM_GFSM_INTERFACE_H_
#define GFSM_GFSM_INTERFACE_H_
#include<mutex>
#include"automaton.h"
#include"acceptor.h"
#include"string_acceptor.h"
#include"transducer.h"
#include"string_transducer.h"
#include"labelvector.h"
#include"cascade.h"
#include"string_cascade.h"
#include"semiring.h"

/// Wrapper classes for Gfsm C library.
/** These classes only cover a small part of the functionality of the
    Gfsm library.  For the most part, they only implement the
    functions that are needed within Norma, so they probably cannot be
    re-used by other projects without some significant work.
 */
namespace Gfsm {

/// Singleton class to create Gfsm objects with a common mutex.
class AutomatonBuilder {
 public:
     /// Get the instance of this singleton.
     static AutomatonBuilder& instance() {
         static AutomatonBuilder my_instance;
         return my_instance;
     }

     /// @see Automaton
     Automaton make_automaton(SemiringType sr = SemiringType::TROPICAL);
     /// @see Acceptor
     Acceptor make_acceptor(SemiringType sr = SemiringType::TROPICAL);
     /// @see StringAcceptor
     StringAcceptor make_stringacceptor(SemiringType sr =
                                        SemiringType::TROPICAL);
     /// @see Transducer
     Transducer make_transducer(SemiringType sr = SemiringType::TROPICAL);
     /// @see StringTransducer
     StringTransducer make_stringtransducer(SemiringType sr =
                                            SemiringType::TROPICAL);
     /// @see Cascade
     Cascade make_cascade(unsigned int depth = 2,
                          SemiringType sr = SemiringType::TROPICAL);
     /// @see StringCascade
     StringCascade make_stringcascade(unsigned int depth = 2,
                                      SemiringType sr = SemiringType::TROPICAL);

 private:
     AutomatonBuilder() = default;
     std::mutex gfsm_mutex;
};
}  // namespace Gfsm

#endif  // GFSM_GFSM_INTERFACE_H_

