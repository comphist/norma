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
#include"automaton.h"
#include<fstream>
#include<stdexcept>
#include<utility>
#include<string>
#include<sstream>
#include<algorithm>
#include<mutex>
#include<set>
#include"gfsmlibs.h"
#include"labelvector.h"
#include"semiring.h"
#include"path.h"

namespace Gfsm {
    Automaton::Automaton(std::mutex* m, SemiringType sr) {
    gfsm_mutex = m;
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    _fsm = gfsm_automaton_new();
    gfsm_automaton_set_semiring_type(_fsm, static_cast<gfsmSRType>(sr));
}

Automaton::Automaton(const Automaton& a) {
    _fsm  = gfsm_automaton_clone(a._fsm);
    this->gfsm_mutex = a.gfsm_mutex;
}

Automaton::Automaton(Automaton&& a) : Automaton(gfsm_mutex) {
    std::swap(_fsm,  a._fsm);
    std::swap(gfsm_mutex, a.gfsm_mutex);
}

Automaton& Automaton::operator=(Automaton a) {
    std::swap(_fsm,  a._fsm);
    std::swap(gfsm_mutex, a.gfsm_mutex);
    return *this;
}

Automaton::~Automaton() throw() {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsm_automaton_free(_fsm);
}

void Automaton::load_binfile(const std::string& filename) {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsmError* err = NULL;
    // _fsm is implicitly cleared, no need to reset manually
    gfsm_automaton_load_bin_filename(_fsm, filename.c_str(), &err);
    if (err != NULL) {
        std::ostringstream msg;
        msg << "error loading gfsm automaton: " << err->message;
        throw std::runtime_error(msg.str());
    }
    _root = gfsm_automaton_get_root(_fsm);
}

void Automaton::save_binfile(const std::string& filename) {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsmError* err = NULL;
    gfsm_automaton_save_bin_filename(_fsm, filename.c_str(), -1, &err);
    if (err != NULL) {
        std::ostringstream msg;
        msg << "error saving gfsm automaton: " << err->message;
        throw std::runtime_error(msg.str());
    }
}

void Automaton::set_semiring_type(SemiringType sr) {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsm_automaton_set_semiring_type(_fsm, static_cast<gfsmSRType>(sr));
}

SemiringType Automaton::get_semiring_type() const {
    return static_cast<SemiringType>(_fsm->sr->type);
}

gfsmStateId Automaton::root() {
    if (_root == gfsmNoState) {  // try fetching it
        _root = gfsm_automaton_get_root(_fsm);
    }
    if (_root == gfsmNoState) {  // try creating it
        _root = gfsm_automaton_n_states(_fsm);
        gfsm_automaton_set_root(_fsm, _root);
    }
    return _root;
}

void Automaton::arcsort() {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsm_automaton_arcsort(_fsm, gfsmASMLower);
}

void Automaton::arcuniq() {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsm_automaton_arcuniq(_fsm);
}

void Automaton::arith_sr_zero_to_zero() {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsm_automaton_arith_state(_fsm, gfsmNoState, gfsmAOMult, 0,
                               gfsmNoLabel, gfsmNoLabel,
                               FALSE, FALSE, TRUE);
}

void Automaton::determinize() {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsm_automaton_determinize(_fsm);
}

void Automaton::minimize(bool remove_eps) {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsm_automaton_minimize_full(_fsm, static_cast<gboolean>(remove_eps));
}

std::set<Path> Automaton::accepted_paths(bool eps_remove) const {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    return find_accepted_paths(eps_remove);
}

std::set<Path> Automaton::find_accepted_paths(bool eps_remove) const {
    std::set<Path> acc;
    if (gfsm_automaton_is_cyclic(_fsm))  // sanity check
        return acc;
    GSList* arcpaths = gfsm_automaton_arcpaths(_fsm);
    GSList* arcpaths_full = arcpaths;
    for (; arcpaths != NULL; arcpaths = arcpaths->next) {
        LabelVector vin, vout;  // should be identical in an Acceptor
        double w = 0.0;
        gfsmArcPath* path = reinterpret_cast<gfsmArcPath*>(arcpaths->data);
        for (unsigned int i = 0; i+1 < path->len; i++) {
            gfsmArc* arc = reinterpret_cast<gfsmArc*>(g_ptr_array_index(path,
                                                                        i));
            if (!eps_remove || arc->lower != EPSILON_LABEL)
                vin.push_back(arc->lower);
            if (!eps_remove || arc->upper != EPSILON_LABEL)
                vout.push_back(arc->upper);
            w += arc->weight;
        }
        /* // check with the original source what this was supposed to do
        if (path->len > 0) {
            gfsmWeight fw = gfsm_ptr2weight(g_ptr_array_index(path,
                                                              path->len-1));
            w = static_cast<double>(fw);
        }*/
        Path p(vin, vout, w);
        acc.insert(p);
    }
    gfsm_arcpath_list_free(arcpaths_full);
    return acc;
}

void Automaton::set_gfsm_automaton(gfsmAutomaton* fsm) {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsm_automaton_free(_fsm);
    _fsm = fsm;
}
}  // namespace Gfsm
