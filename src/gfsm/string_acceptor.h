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
#ifndef GFSM_STRING_ACCEPTOR_H_
#define GFSM_STRING_ACCEPTOR_H_
#include<set>
#include<utility>
#include<vector>
#include"acceptor.h"
#include"alphabet.h"
#include"string_impl.h"

namespace Gfsm {
class Alphabet;

/// An Acceptor with an Alphabet.
class StringAcceptor : public Acceptor {
 public:
    explicit StringAcceptor(const Alphabet& alph)
        : Acceptor(), _alph(alph) {}
    StringAcceptor() : Acceptor() {}
    StringAcceptor(const StringAcceptor& a);
    StringAcceptor(StringAcceptor&& a)
        : Acceptor(std::move(a)), _alph(std::move(a._alph)) {}
    StringAcceptor& operator=(StringAcceptor a);
    ~StringAcceptor() = default;

    /// Set the Alphabet for this acceptor.
    void set_alphabet(const Alphabet& alph);
    /// Get the Alphabet for this acceptor.
    const Alphabet& get_alphabet() const;

    using Acceptor::accepts;
    /// Test if automaton is in a final state after reading the given symbols.
    /** Uses Alphabet::map_symbols() to map symbols to a LabelVector.
     */
    bool accepts(const string_impl& str) const;
    /// Test if automaton is in a final state after reading the given symbols.
    /** Uses Alphabet::map_symbols() to map symbols to a LabelVector.
     */
    bool accepts(const std::vector<string_impl>& str) const;

    /// Find all symbol sequences accepted by this automaton.
    /** @return A set of accepted symbol sequences, concatenated into
                a single string.
    */
    std::set<string_impl> accepted() const;
    /// Find all symbol sequences accepted by this automaton.
    /** @return A set of accepted symbol sequences, as a vector of symbols.
    */
    std::set<std::vector<string_impl>> accepted_vectors() const;

    /// Add a word to the automaton.
    /** @param str The word that should be accepted by the automaton.
                   Each character of the string is interpreted as one symbol.
                   If the word contains symbols that are not in the alphabet,
                   the alphabet is extended automatically.
        @param partials If true, all substrings of str starting at index 0
                        will also be accepted (e.g., if str = "foo", the
                        automaton will also accept "f" and "fo").
     */
    void add_word(const string_impl& str, bool partials = false);
    /// Add a word to the automaton.
    /** @param str The word that should be accepted by the automaton.
                   If the word contains symbols that are not in the alphabet,
                   the alphabet is extended automatically.
        @param partials If true, all substrings of str starting at index 0
                        will also be accepted (e.g., if str = {"f", "o", "o"},
                        the automaton will also accept {"f"} and {"f", "o"}).
     */
    void add_word(const std::vector<string_impl>& str, bool partials = false);

 protected:
    Alphabet _alph;
};

}  // namespace Gfsm

#endif  // GFSM_STRING_ACCEPTOR_H_
