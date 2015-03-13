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
#ifndef GFSM_STRING_CASCADE_H_
#define GFSM_STRING_CASCADE_H_
#include<mutex>
#include<set>
#include<vector>
#include"gfsmlibs.h"
#include"alphabet.h"
#include"string_acceptor.h"
#include"string_transducer.h"
#include"cascade.h"
#include"path.h"
#include"string_impl.h"

namespace Norma {
namespace Normalizer {
class Lexicon;
}  // namespace Normalizer
}  // namespace Norma

namespace Gfsm {
class AutomatonBuilder;

/// A Cascade with an Alphabet.
class StringCascade : public Cascade {
    friend class AutomatonBuilder;
 public:
    StringCascade(const StringCascade& a)
        : Cascade(a), _alph_in(a._alph_in), _alph_out(a._alph_out) {}
    StringCascade(StringCascade&& a)
        : Cascade(std::move(a)), _alph_in(std::move(a._alph_in)),
          _alph_out(std::move(a._alph_out)) {}
    StringCascade& operator=(StringCascade a);
    ~StringCascade() = default;

    /// Append an automaton to the cascade.
    /** @see Cascade::append() */
    void append(const StringAcceptor& a);
    /// Append an automaton to the cascade.
    /** @see Cascade::append() */
    void append(const StringTransducer& a);
    void append(Norma::Normalizer::Lexicon* lex);

    /// Finds the n-best paths for a given input sequence.
    /** @see Cascade::lookup_nbest(const LabelVector&) const */
    std::set<StringPath> lookup_nbest(const string_impl& str) const;
    /// Finds the n-best paths for a given input sequence.
    /** @see Cascade::lookup_nbest(const LabelVector&) const */
    std::set<StringPath> lookup_nbest(const std::vector<string_impl>& str)
                                                                     const;
    /// Finds the n-best paths for a given input sequence.
    /** @see Cascade::lookup_nbest(const LabelVector&, unsigned int, double) */
    std::set<StringPath> lookup_nbest(const string_impl& str,
                                      unsigned int max_paths,
                                      double max_weight);
    /// Finds the n-best paths for a given input sequence.
    /** @see Cascade::lookup_nbest(const LabelVector&, unsigned int, double) */
    std::set<StringPath> lookup_nbest(const std::vector<string_impl>& str,
                                      unsigned int max_paths,
                                      double max_weight);

 protected:
    StringCascade(std::mutex* m, unsigned int depth = 2,
                  SemiringType sr = SemiringType::TROPICAL)
        : Cascade(m, depth, sr) {}

    Alphabet _alph_in;
    Alphabet _alph_out;

    std::set<StringPath> find_map_nbest(const LabelVector& vec) const;
};

}  // namespace Gfsm

#endif  // GFSM_STRING_CASCADE_H_

