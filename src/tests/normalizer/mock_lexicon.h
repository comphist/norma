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
#ifndef TESTS_NORMALIZER_MOCK_LEXICON_H_
#define TESTS_NORMALIZER_MOCK_LEXICON_H_
#include<algorithm>
#include<map>
#include<string>
#include<vector>
#include"string_impl.h"
#include"normalizer/lexicon_interface.h"

using Norma::Normalizer::LexiconInterface;

class MockLexicon : public LexiconInterface {
 private:
     const std::vector<string_impl> _words {
         "eins", "zwei", "drei", "und"
     };
     const std::vector<string_impl> _partial_words {
         "",
         "e", "z", "d", "u",
         "ei", "zw", "dr", "un",
         "ein", "zwe", "dre", "und",
         "eins", "zwei", "drei"
     };

     void do_init() {}
     void do_clear() {}
     void do_set_from_params(const std::map<std::string, std::string>&
                             params) {}
     void do_save_params() {}
     bool check_contains(const string_impl& word) const {
         return (std::find(_words.begin(), _words.end(), word) != _words.end());
     }
     bool check_contains_partial(const string_impl& word) const {
         return (std::find(_partial_words.begin(), _partial_words.end(), word)
                 != _partial_words.end());
     }
     bool add_word(const string_impl& word) { return true; }
     std::vector<string_impl> retrieve_all_entries() const {
         return _words;
     }
     unsigned int get_size() const {
         return _words.size();
     }
};

#endif  // TESTS_NORMALIZER_MOCK_LEXICON_H_
