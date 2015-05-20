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
#ifndef GFSM_AUTOMATON_H_
#define GFSM_AUTOMATON_H_
#include<string>
#include<set>
#include"gfsmlibs.h"
#include"semiring.h"
#include"path.h"

namespace Gfsm {
class LabelVector;
class Cascade;

/// A finite-state automaton.
/** Implements functions that apply to all types of finite-state
    automata.  When creating new automata, it is probably better to
    use the specialized Acceptor or Transducer classes.
 */
class Automaton {
    friend class Cascade;
 public:
    Automaton() : Automaton(SemiringType::TROPICAL) {}
    explicit Automaton(SemiringType sr);
    Automaton(const Automaton& a);
    Automaton(Automaton&& a);
    Automaton& operator=(Automaton a);
    ~Automaton() throw();
    /// Load a Gfsm automaton from a binary file.
    void load_binfile(const std::string& filename);
    /// Save the Gfsm automaton to a binary file.
    void save_binfile(const std::string& filename);

    void set_semiring_type(SemiringType sr);
    SemiringType get_semiring_type() const;

    /// Sort all arcs in the automaton.
    void arcsort();
    /// Collect weights on adjacent, otherwise identical arcs.
    /** Should only be called on an arc-sorted automaton.
     */
    void arcuniq();
    /// Multiply all zero weights with semiring-zero.
    void arith_sr_zero_to_zero();
    /// Determinize the automaton.
    void determinize();
    /// Minimize the automaton.
    /** @param remove_eps Whether to perform epsilon removal.
     */
    void minimize(bool remove_eps = true);

    /// Find all paths that are accepted by this automaton.
    /** If the automaton is cyclic, an empty set is returned, as the
        set of accepted paths would be infinite in this case.
     */
    std::set<Path> accepted_paths() const;

    /// Make sure automaton has a root state
    void ensure_root() { root(); }

 protected:
    gfsmAutomaton* _fsm;             /**< Pointer to automaton object. */
    gfsmStateId _root = gfsmNoState; /**< ID of the root state. */

    void set_gfsm_automaton(gfsmAutomaton* fsm);

    /// Return ID of the root state; creates a root state if none exists.
    gfsmStateId root();

};

}  // namespace Gfsm

#endif  // GFSM_AUTOMATON_H_

