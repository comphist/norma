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
#ifndef GFSM_ACCEPTOR_H_
#define GFSM_ACCEPTOR_H_
#include<utility>
#include<mutex>
#include<set>
#include"automaton.h"

namespace Gfsm {
class LabelVector;
class AutomatonBuilder;

/// A finite-state acceptor.
/** Implements functions specific to a finite-state acceptor, i.e., an
    Automaton where input labels always equal output labels.
 */
class Acceptor : public Automaton {
    friend class AutomatonBuilder;
 public:
    Acceptor(const Acceptor& a) : Automaton(a) {}
    Acceptor(Acceptor&& a) : Automaton(std::move(a)) {}
    Acceptor& operator=(Acceptor a);
    ~Acceptor() = default;

    /// Test if automaton is in a final state after reading a given LabelVector.
    /** @param vec LabelVector to test.
     */
    bool accepts(const LabelVector& vec) const;

    /// Find all LabelVectors accepted by this automaton.
    /** @see Automaton::accepted_paths()
     */
    std::set<LabelVector> accepted() const;

    /// Add a path to the automaton.
    /** @param vec The LabelVector that should be accepted by the
                   automaton.
        @param set_all_final If true, all subvectors of vec starting at
                   index 0 will also be accepted (e.g., if vec = {1, 3, 5},
                   the automaton will also accept {1} and {1, 3}).
     */
    void add_path(const LabelVector& vec, bool set_all_final = false);

 protected:
    Acceptor() = delete;
    explicit Acceptor(std::mutex* m) : Automaton(m)
        { _fsm->flags.is_transducer = FALSE; }
};

}  // namespace Gfsm

#endif  // GFSM_ACCEPTOR_H_
