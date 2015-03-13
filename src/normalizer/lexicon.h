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
#ifndef NORMALIZER_LEXICON_H_
#define NORMALIZER_LEXICON_H_
#include<map>
#include<string>
#include<vector>
#include<boost/filesystem.hpp>  // NOLINT[build/include_order]
#include"gfsm_wrapper.h"
#include"string_impl.h"
#include"normalizer/lexicon_interface.h"

namespace Norma {
namespace Normalizer {

class Lexicon : public LexiconInterface {
friend class Gfsm::StringCascade;
 public:
     Lexicon() {}
     ~Lexicon() {
         if (_fsm != nullptr)
             delete _fsm;
     }

     void set_lexfile(const std::string& fn) {
         _lexfile = boost::filesystem::path(fn);
     }
     std::string get_lexfile() const { return _lexfile.string(); }
     void set_symfile(const std::string& fn) {
         _symfile = boost::filesystem::path(fn);
     }
     std::string get_symfile() const { return _symfile.string(); }

     /// perform (possibly time-intensive) FST optimizations
     void optimize();

     static const string_impl SYMBOL_BOUNDARY;
     static const string_impl SYMBOL_ANY;
     static const string_impl SYMBOL_EPSILON;

     const Gfsm::Alphabet& get_alphabet() const;

 protected:
     Gfsm::StringAcceptor* get_acceptor() const { return _fsm; }

 private:
     Gfsm::AutomatonBuilder& gfsm_builder
         = Gfsm::AutomatonBuilder::instance();
     boost::filesystem::path _lexfile;
     boost::filesystem::path _symfile;
     Gfsm::StringAcceptor* _fsm = nullptr;

     gfsmLabelVal _label_boundary;
     gfsmLabelVal _label_any;
     gfsmLabelVal _label_epsilon;
     Gfsm::Alphabet init_alphabet();

     void do_init();
     void do_clear();
     void do_set_from_params(const std::map<std::string, std::string>&
                             params);
     void do_save_params();
     bool check_contains(const string_impl& word) const;
     bool check_contains_partial(const string_impl& word) const;
     bool add_word(const string_impl& word);
     std::vector<string_impl> retrieve_all_entries() const;
     unsigned int get_size() const;
};

}  // namespace Normalizer
}  // namespace Norma

#endif  // NORMALIZER_LEXICON_H_
