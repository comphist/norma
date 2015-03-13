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
#ifndef GFSM_TRANSDUCER_H_
#define GFSM_TRANSDUCER_H_
#include<utility>
#include<mutex>
#include<set>
#include<tuple>
#include"automaton.h"
#include"path.h"

namespace Gfsm {
class LabelVector;
class AutomatonBuilder;

/// A finite-state transducer.
/** Implements functions specific to a finite-state transducer, i.e.,
    an Automaton which returns an output sequence for a given input
    sequence of labels.
 */
class Transducer : public Automaton {
    friend class AutomatonBuilder;
 public:
    Transducer(const Transducer& a) : Automaton(a) {}
    Transducer(Transducer&& a) : Automaton(std::move(a)) {}
    Transducer& operator=(Transducer a);
    ~Transducer() = default;

    /// Transduce a given input sequence.
    /** @param input Input sequence as a LabelVector
        @return A set of Path objects accepted by this transducer given the
                input sequence.  Only paths ending in a final state are
                returned.
     */
    std::set<Path> transduce(const LabelVector& input) const;

    /// Add a new path to the transducer.
    /** @param p The Path to be added
        @param cyclic If true, the final transition will return to the root
                      state of the automaton, making it cyclic.
        @param final  If true, the state after the final transition will be
                      turned into a final state.
     */
    void add_path(const Path& p, bool cyclic = false, bool final = true);

    /// Add a new cyclic path to the transducer.
    /** Convenience function for add_path(p, true, final).
        @see add_path()
     */
    void add_cyclic_path(const Path& p, bool final = true)
        { add_path(p, true, final); }

 protected:
    Transducer() = delete;
    explicit Transducer(std::mutex* m) : Automaton(m)
        { _fsm->flags.is_transducer = TRUE; }
};

}  // namespace Gfsm

#endif  // GFSM_TRANSDUCER_H_
