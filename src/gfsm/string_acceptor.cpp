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
#include"string_acceptor.h"
#include<stdexcept>
#include<algorithm>
#include<set>
#include<vector>
#include"labelvector.h"
#include"alphabet.h"

namespace Gfsm {

StringAcceptor::StringAcceptor(const StringAcceptor& a)
        : Acceptor(a), _alph(a._alph) {}

StringAcceptor& StringAcceptor::operator=(StringAcceptor a) {
    Automaton::operator=(a);
    std::swap(_alph,  a._alph);
    return *this;
}

void StringAcceptor::set_alphabet(const Alphabet& alph) {
    _alph = alph;
}

const Alphabet& StringAcceptor::get_alphabet() const {
    return _alph;
}

bool StringAcceptor::accepts(const string_impl& str) const {
    try {
        LabelVector v = _alph.map_symbols(str);
        return accepts(v);
    }
    catch (const std::out_of_range& err) {
        return false;
    }
}

bool StringAcceptor::accepts(const std::vector<string_impl>& str) const {
    try {
        LabelVector v = _alph.map_symbols(str);
        return accepts(v);
    }
    catch (const std::out_of_range& err) {
        return false;
    }
}

std::set<string_impl> StringAcceptor::accepted() const {
    std::set<LabelVector> labels = Acceptor::accepted();
    std::set<string_impl> acc;
    for (const LabelVector& vec : labels) {
        acc.insert(_alph.map_labels(vec));
    }
    return acc;
}

std::set<std::vector<string_impl>> StringAcceptor::accepted_vectors() const {
    std::set<LabelVector> labels = Acceptor::accepted();
    std::set<std::vector<string_impl>> acc;
    for (const LabelVector& vec : labels) {
        acc.insert(_alph.map_labels_to_vector(vec));
    }
    return acc;
}

void StringAcceptor::add_word(const string_impl& str, bool partials) {
    LabelVector v = _alph.cover(str);
    add_path(v, partials);
}

void StringAcceptor::add_word(const std::vector<string_impl>& str,
                              bool partials) {
    LabelVector v = _alph.cover(str);
    add_path(v, partials);
}

}  // namespace Gfsm
