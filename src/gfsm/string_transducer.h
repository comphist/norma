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
#ifndef GFSM_STRING_TRANSDUCER_H_
#define GFSM_STRING_TRANSDUCER_H_
#include<set>
#include<utility>
#include<vector>
#include"alphabet.h"
#include"transducer.h"
#include"string_impl.h"

namespace Gfsm {
class Alphabet;

/// A Transducer with an Alphabet.
class StringTransducer : public Transducer {
 public:
    StringTransducer(const Alphabet& alph_in,
                     const Alphabet& alph_out)
        : Transducer(), _alph_in(alph_in), _alph_out(alph_out) {}
    StringTransducer() : Transducer() {}
    StringTransducer(const StringTransducer& a)
        : Transducer(a), _alph_in(a._alph_in), _alph_out(a._alph_out) {}
    StringTransducer(StringTransducer&& a)
        : Transducer(std::move(a)), _alph_in(std::move(a._alph_in)),
          _alph_out(std::move(a._alph_out)) {}
    StringTransducer& operator=(StringTransducer a);
    ~StringTransducer() = default;

    /// Set the Alphabet for input symbols.
    void set_input_alphabet(const Alphabet& alph);
    /// Set the Alphabet for output symbols.
    void set_output_alphabet(const Alphabet& alph);
    /// Get the Alphabet for input symbols.
    const Alphabet& get_input_alphabet() const;
    /// Get the Alphabet for output symbols.
    const Alphabet& get_output_alphabet() const;

    using Transducer::transduce;
    /// Transduce a given input sequence.
    /** @param str Input sequence as a string where each character is
                   interpreted as a symbol.
        @return A set of StringPath objects accepted by this transducer
                given the input sequence.
    */
    std::set<StringPath> transduce(const string_impl& str) const;
    /// Transduce a given input sequence.
    /** @param str Input sequence as a vector of symbols.
        @return A set of StringPath objects accepted by this transducer
                given the input sequence.
    */
    std::set<StringPath> transduce(const std::vector<string_impl>& str) const;

    /// Add a new path to the transducer
    /** Identical to Transducer::add_path(), but constructs the Path object
        from the given arguments first.
        @param str_in Input sequence as a string of symbols
        @param str_out Output sequence as a string of symbols
        @param weight Weight of the path
        @param cyclic See Transducer::add_path()
        @param final See Transducer::add_path()
     */
    void add_path(const string_impl& str_in,
                  const string_impl& str_out,
                  double weight,
                  bool cyclic = false, bool final = true);
    /// Add a new path to the transducer.
    /** Identical to Transducer::add_path(), but takes a StringPath as first
        argument.
    */
    void add_path(const StringPath& path,
                  bool cyclic = false, bool final = true);
    /// Add a new cyclic path to the transducer.
    /** Convenience function for add_path() with cyclic=true. */
    void add_cyclic_path(const string_impl& str_in,
                         const string_impl& str_out,
                         double weight,
                         bool final = true)
        { add_path(str_in, str_out, weight, true, final); }
    /// Add a new cyclic path to the transducer.
    /** Convenience function for add_path() with cyclic=true. */
    void add_cyclic_path(const StringPath& path, bool final = true)
        { add_path(path, true, final); }

 protected:
    Alphabet _alph_in;
    Alphabet _alph_out;

    std::set<StringPath> transduce_vector_to_string(const LabelVector& vec)
        const;
};

}  // namespace Gfsm

#endif  // GFSM_STRING_TRANSDUCER_H_
