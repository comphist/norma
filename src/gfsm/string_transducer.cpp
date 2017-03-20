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
#include"string_transducer.h"
#include<algorithm>
#include<set>
#include<vector>
#include"gfsmlibs.h"
#include"labelvector.h"
#include"path.h"
#include"string_impl.h"

namespace Gfsm {

StringTransducer& StringTransducer::operator=(StringTransducer a) {
    Automaton::operator=(a);
    std::swap(_alph_in, a._alph_in);
    std::swap(_alph_out, a._alph_out);
    return *this;
}

void StringTransducer::set_input_alphabet(const Alphabet& alph) {
    _alph_in = alph;
}

void StringTransducer::set_output_alphabet(const Alphabet& alph) {
    _alph_out = alph;
}

const Alphabet& StringTransducer::get_input_alphabet() const {
    return _alph_in;
}

const Alphabet& StringTransducer::get_output_alphabet() const {
    return _alph_out;
}

std::set<StringPath> StringTransducer::transduce(const string_impl& str) const {
    try {
        LabelVector v = _alph_in.map_symbols(str);
        return transduce_vector_to_string(v);
    }
    catch (const std::out_of_range& err) {
        return std::set<StringPath>();
    }
}

std::set<StringPath>
StringTransducer::transduce(const std::vector<string_impl>& str) const {
    try {
        LabelVector v = _alph_in.map_symbols(str);
        return transduce_vector_to_string(v);
    }
    catch (const std::out_of_range& err) {
        return std::set<StringPath>();
    }
}

std::set<StringPath>
StringTransducer::transduce_vector_to_string(const LabelVector& vec) const {
    std::set<StringPath> x;
    auto results = Transducer::transduce(vec);
    for (const Path& p : results) {
        try {
            x.insert(StringPath::from(p, _alph_in, _alph_out));
        }
        catch (const std::out_of_range& err) {}
    }
    return x;
}

void StringTransducer::add_path(const string_impl& str_in,
                                const string_impl& str_out,
                                double weight, bool cyclic, bool final) {
    Transducer::add_path(Path(_alph_in.map_symbols(str_in),
                              _alph_out.map_symbols(str_out),
                              weight),
                         cyclic, final);
}

void StringTransducer::add_path(const StringPath& path,
                                bool cyclic, bool final) {
    Transducer::add_path(Path(_alph_in.map_symbols(path.get_input()),
                              _alph_out.map_symbols(path.get_output()),
                              path.get_weight()),
                         cyclic, final);
}

}  // namespace Gfsm
