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
#include"cascade.h"
#include<algorithm>
#include<mutex>
#include<set>
#include"gfsmlibs.h"
#include"automaton.h"
#include"semiring.h"
#include"path.h"

namespace Gfsm {
    Cascade::Cascade(std::mutex* m, unsigned int depth, SemiringType sr) {
    gfsm_mutex = m;
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    _csc = gfsmxl_cascade_new_full(depth, static_cast<gfsmSRType>(sr));
    _cl  = gfsmxl_cascade_lookup_new();
    gfsmxl_cascade_clear(_csc, FALSE);
    gfsmxl_cascade_lookup_set_cascade(_cl, _csc);
}

Cascade::Cascade(const Cascade& a) {
    this->gfsm_mutex = a.gfsm_mutex;
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    // there is no clone function, so we have to do this manually ...
    _csc = gfsmxl_cascade_new_full(a._csc->depth, a._csc->sr->type);
    gfsmxl_cascade_clear(_csc, FALSE);
    // clone each automaton in Cascade 'a' and append it to this one
    for (unsigned int i = 0; i < a._csc->depth; ++i) {
        gfsmIndexedAutomaton* ptr = gfsmxl_cascade_index(a._csc, i);
        if (ptr != NULL) {
            gfsmxl_cascade_append_indexed(_csc,
                                          gfsm_indexed_automaton_clone(ptr));
        }
    }
    // recreate lookup object
    _cl = gfsmxl_cascade_lookup_new_full(_csc, a._cl->max_w, a._cl->max_paths,
                                         a._cl->max_ops);
}

Cascade::Cascade(Cascade&& a) : Cascade(gfsm_mutex) {
    std::swap(_cl,  a._cl);
    std::swap(_csc, a._csc);
    std::swap(gfsm_mutex, a.gfsm_mutex);
}

Cascade& Cascade::operator=(Cascade a) {
    std::swap(_cl,  a._cl);
    std::swap(_csc, a._csc);
    std::swap(gfsm_mutex, a.gfsm_mutex);
    return *this;
}

Cascade::~Cascade() throw() {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    // this also frees _csc and its indexed automata:
    if (_cl != nullptr)
        gfsmxl_cascade_lookup_free(_cl);
}

unsigned int Cascade::get_max_paths() const {
    return _cl->max_paths;
}

unsigned int Cascade::get_max_ops() const {
    return _cl->max_ops;
}

double Cascade::get_max_weight() const {
    return _cl->max_w;
}

void Cascade::set_max_paths(unsigned int n) {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    _cl->max_paths = n;
}

void Cascade::set_max_ops(unsigned int n) {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    _cl->max_ops = n;
}

void Cascade::set_max_weight(double w) {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    _cl->max_w = w;
}

void Cascade::append(const Automaton* a) {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsmIndexedAutomaton* xfsm = gfsm_automaton_to_indexed(a->_fsm, NULL);
    gfsmxl_cascade_append_indexed(_csc, xfsm);
    _size++;
}

void Cascade::sort() {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsmArcCompMask mask = gfsm_acmask_from_args(gfsmACLower, gfsmACWeight,
                                                 gfsmACUpper, gfsmACTarget,
                                                 gfsmACNone);
    gfsmxl_cascade_sort_all(_csc, mask);
}

std::set<Path> Cascade::lookup_nbest(const LabelVector& v) const {
    std::lock_guard<std::mutex> guard(*gfsm_mutex);
    gfsmAutomaton* result_fsm = gfsmxl_cascade_lookup_nbest(_cl, v._vec, NULL);
    std::mutex dummy;
    Gfsm::Automaton result(&dummy, static_cast<SemiringType>(_csc->sr->type));
    result.set_gfsm_automaton(result_fsm);
    return result.find_accepted_paths(true);
}

std::set<Path> Cascade::lookup_nbest(const LabelVector& v,
                                     unsigned int max_paths,
                                     double max_weight) {
    set_max_paths(max_paths);
    set_max_weight(max_weight);
    return lookup_nbest(v);
}
}  // namespace Gfsm

