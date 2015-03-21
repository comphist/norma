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
#include"string_cascade.h"
#include<algorithm>
#include<mutex>
#include<set>
#include<vector>
#include"gfsmlibs.h"
#include"alphabet.h"
#include"labelvector.h"
#include"string_acceptor.h"
#include"string_transducer.h"
#include"path.h"
#include"lexicon.h"

using Norma::Normalizer::Lexicon;

namespace Gfsm {

StringCascade& StringCascade::operator=(StringCascade a) {
    Cascade::operator=(a);
    std::swap(_alph_in, a._alph_in);
    std::swap(_alph_out, a._alph_out);
    return *this;
}

void StringCascade::append(const StringAcceptor& a) {
    // TODO(bollmann): could check for compatibility of the alphabets?
    if (_size == 0)
        _alph_in = a.get_alphabet();
    Cascade::append(&a);
    _alph_out = a.get_alphabet();
}

void StringCascade::append(const StringTransducer& a) {
    // TODO(bollmann): could check for compatibility of the alphabets?
    if (_size == 0)
        _alph_in = a.get_input_alphabet();
    Cascade::append(&a);
    _alph_out = a.get_output_alphabet();
}

void StringCascade::append(Lexicon* lex) {
    StringAcceptor* acceptor = lex->get_acceptor();
    append(*acceptor);
}

std::set<StringPath> StringCascade::lookup_nbest(const string_impl& str) const {
    return find_map_nbest(_alph_in.map_symbols(str));
}

std::set<StringPath>
StringCascade::lookup_nbest(const std::vector<string_impl>& str) const {
    return find_map_nbest(_alph_in.map_symbols(str));
}

std::set<StringPath>
StringCascade::find_map_nbest(const LabelVector& vec) const {
    std::set<StringPath> results;
    std::set<Path> paths = Cascade::lookup_nbest(vec);
    for (const Path& p : paths) {
        results.insert(StringPath::from(p, _alph_in, _alph_out));
    }
    return results;
}

std::set<StringPath>
StringCascade::lookup_nbest(const std::vector<string_impl>& str,
                            unsigned int max_paths, double max_weight) {
    set_max_paths(max_paths);
    set_max_weight(max_weight);
    return lookup_nbest(str);
}

std::set<StringPath>
StringCascade::lookup_nbest(const string_impl& str,
                            unsigned int max_paths, double max_weight) {
    set_max_paths(max_paths);
    set_max_weight(max_weight);
    return lookup_nbest(str);
}
}  // namespace Gfsm
