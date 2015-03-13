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
#ifndef GFSM_ALPHABET_H_
#define GFSM_ALPHABET_H_
#include<set>
#include<vector>
#include<string>
#include<boost/bimap.hpp>  // NOLINT[build/include_order]
#include"gfsmlibs.h"
#include"string_impl.h"

namespace Gfsm {
class LabelVector;

/// A bi-directional dictionary mapping (numeric) labels to (string) symbols.
/** @see StringAcceptor
    @see StringTransducer
    @see StringCascade
 */
class Alphabet {
 public:
    typedef const string_impl (*unknown_symbol_mapper)(const string_impl&);

    /// Clear the alphabet.
    void clear();
    /// Load an alphabet from a text file.
    void load_labfile(const std::string& filename);
    /// Save the alphabet to a text file.
    void save_labfile(const std::string& filename) const;
    /// Set a mapper function to use when encountering unknown symbols.
    void set_unknown_mapper(unknown_symbol_mapper funct) {
        _map_unknown = funct;
    }
    /// If set to true, ignores unknown symbols.
    void set_ignore_unknowns(bool ignore) {
        _ignore_unknowns = ignore;
    }
    bool is_ignore_unknowns() { return _ignore_unknowns; }

    /// Check if the alphabet covers a given symbol.
    bool contains(const string_impl& symbol) const;
    /// Check if the alphabet covers a given label.
    bool contains(const gfsmLabelVal& label) const;
    /// Get the label corresponding to a given symbol.
    /** Throws std::out_of_range if the alphabet doesn't contain the symbol.
     */
    gfsmLabelVal get_label(const string_impl& symbol) const;
    /// Get the symbol corresponding to a given label.
    /** Throws std::out_of_range if the alphabet doesn't contain the label.
     */
    string_impl get_symbol(const gfsmLabelVal& label) const;
    /// Map a given symbol to a (numeric) label.
    /** Returns the label corresponding to the given symbol; contrary
        to get_label(), if the alphabet doesn't contain the symbol,
        the mapping function set with set_unknown_mapper() is called.
        If the mapping function also returns an invalid symbol, this
        function returns 0.
     */
    gfsmLabelVal map_symbol(const string_impl& symbol) const;
    /// Map a string with several symbols to a LabelVector.
    /** Calls map_symbol() for each character in the given string, and
        combines the results into a LabelVector.  If is_ignore_unknowns()
        is false, label values of 0 will be included in the returned
        LabelVector; otherwise they are discarded.
     */
    LabelVector map_symbols(const string_impl& symbols) const;
    /// Map a vector of symbols to a LabelVector.
    /** Calls map_symbol() for each element of the given vector, and
        combines the results into a LabelVector.  If is_ignore_unknowns()
        is false, label values of 0 will be included in the returned
        LabelVector; otherwise they are discarded.
     */
    LabelVector map_symbols(const std::vector<string_impl>& symbols) const;
    /// Map a LabelVector to a string.
    /** Calls get_symbol() for each element of the given LabelVector,
        and concatenates the returned symbols to a single string.
        Invalid label values are ignored.
     */
    string_impl map_labels(const LabelVector& labels) const;
    /// Map a LabelVector to a vector of symbols.
    /** Calls get_symbol() for each element of the given LabelVector,
        and returns a vector of those symbols.  Invalid label values
        are ignored.
     */
    std::vector<string_impl> map_labels_to_vector
        (const LabelVector& labels) const;

    /// Add a new mapping to the alphabet.
    void add_mapping(const string_impl& symbol,
                     const gfsmLabelVal& label);
    /// Add a new symbol to the alphabet.
    /** If the symbol is not already in the alphabet, chooses a new
        free label and adds the (symbol, label) pair to the alphabet.
        @return The label assigned to this symbol.
     */
    gfsmLabelVal add_symbol(const string_impl& symbol);

    /// Ensure that a given set of symbols is covered by the alphabet.
    /** Adds new mappings for unknown symbols as needed.
        @param symbols A string where each character is interpreted
                       as a symbol.
        @return A LabelVector containing the corresponding labels for
                each symbol in the input.
     */
    LabelVector cover(const string_impl& symbols);
    /// Ensure that a given set of symbols is covered by the alphabet.
    LabelVector cover(const std::vector<string_impl>& symbols);
    /// Ensure that a given set of symbols is covered by the alphabet.
    LabelVector cover(const std::set<string_impl>& symbols);
    /// Get the set of all covered symbols.
    /** Returns the set of all symbols for which contains() returns true.
        @see contains(const string_impl&) const
     */
    std::set<string_impl> covered() const;

 private:
    typedef boost::bimap<string_impl, gfsmLabelVal> AlphabetBimap;
    typedef AlphabetBimap::value_type AlphabetMapping;
    AlphabetBimap _alph;
    unknown_symbol_mapper _map_unknown = nullptr;
    bool _ignore_unknowns = false;
    gfsmLabelVal _next_free_label = 1;
};

}  // namespace Gfsm

#endif  // GFSM_ALPHABET_H_
