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
#include"transducer.h"
#include<algorithm>
#include<mutex>
#include<tuple>
#include<set>
#include"gfsmlibs.h"
#include"labelvector.h"
#include"path.h"
#include"semiring.h"

namespace Gfsm {

Transducer& Transducer::operator=(Transducer a) {
    Automaton::operator=(a);
    return *this;
}

std::set<Path> Transducer::transduce(const LabelVector& input) const {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    std::set<Path> tr;
    gfsmAutomaton* result = gfsm_automaton_new();
    gfsm_automaton_lookup(_fsm, input._vec, result);
    std::mutex dummy;  // we are creating new Gfsm objects, but we are
                       // already under lock_guard
    if (gfsm_automaton_n_final_states(result) > 0) {
        Transducer a(&dummy);
        a.set_gfsm_automaton(result);
        tr = a.find_accepted_paths();
    } else {
        gfsm_automaton_free(result);
    }
    return tr;
}

void Transducer::add_path(const Path& p, bool cyclic, bool final) {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsmStateId from = root();
    gfsmStateId to   = gfsm_automaton_n_states(_fsm);
    gfsmWeight  w    = p.get_weight();
    unsigned int input_size  = p.get_input().size(),
                 output_size = p.get_output().size(),
                 max_size    = std::max(input_size, output_size);
    for (unsigned int i = 0; i < max_size; ++i) {
        gfsmLabelVal from_val = (i < input_size) ? p.get_input().get(i) : 0;
        gfsmLabelVal to_val   = (i < output_size) ? p.get_output().get(i) : 0;
        if (cyclic && (i == max_size - 1)) {
            to = root();  // cyclic: back to the root
        }
        gfsm_automaton_add_arc(_fsm, from, to, from_val, to_val, w);
        w    = _fsm->sr->one;
        from = to;
        to++;
    }
    if (final && (gfsm_automaton_state_is_final(_fsm, from) == FALSE)) {
        gfsm_automaton_set_final_state_full(_fsm, from, TRUE, _fsm->sr->one);
    }
}
}  // namespace Gfsm
