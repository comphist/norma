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
#include"acceptor.h"
#include<mutex>
#include<set>
#include"gfsmlibs.h"
#include"labelvector.h"
#include"path.h"

namespace Gfsm {

Acceptor& Acceptor::operator=(Acceptor a) {
    Automaton::operator=(a);
    return *this;
}

bool Acceptor::accepts(const LabelVector& vec) const {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsmAutomaton* result = gfsm_automaton_new();
    gfsm_automaton_lookup(_fsm, vec._vec, result);
    bool is_accepted = (gfsm_automaton_n_final_states(result) > 0);
    gfsm_automaton_free(result);
    return is_accepted;
}

std::set<LabelVector> Acceptor::accepted() const {
    std::set<Path> paths = accepted_paths();
    std::set<LabelVector> acc;
    for (const Path& p : paths) {
        acc.insert(p.get_input());
    }
    return acc;
}

void Acceptor::add_path(const LabelVector& vec, bool set_all_final) {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsmStateId from = root();
    gfsmWeight  one  = _fsm->sr->one;
    for (gfsmLabelVal value : vec) {
        gfsmArcIter iter;
        gfsmStateId to = gfsmNoState;
        gfsm_arciter_open(&iter, _fsm, from);
        while (gfsm_arciter_ok(&iter)) {
            gfsmArc* arc = gfsm_arciter_arc(&iter);
            // several assumptions are made here:
            // Acceptor must be deterministic, epsilon-free, and unweighted
            if (arc->lower == value) {
                to = arc->target;
                break;
            }
            gfsm_arciter_next(&iter);
        }
        if (to == gfsmNoState) {  // need to create a new node & arc
            to = gfsm_automaton_n_states(_fsm);
            gfsm_automaton_add_arc(_fsm, from, to, value, value, one);
        }
        // set_all_final set?
        if (set_all_final &&
              gfsm_automaton_state_is_final(_fsm, to) == FALSE)
            gfsm_automaton_set_final_state_full(_fsm, to, TRUE, one);
        // follow arc
        from = to;
        gfsm_arciter_close(&iter);
    }

    // accepting the path requires the last state to be final
    if (gfsm_automaton_state_is_final(_fsm, from) == FALSE)
        gfsm_automaton_set_final_state_full(_fsm, from, TRUE, one);
}

}  // namespace Gfsm
