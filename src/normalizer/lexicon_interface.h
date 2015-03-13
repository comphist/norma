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
#ifndef NORMALIZER_LEXICON_INTERFACE_H_
#define NORMALIZER_LEXICON_INTERFACE_H_
#include<map>
#include<string>
#include<vector>
#include"string_impl.h"

namespace Norma {
namespace Normalizer {

class LexiconInterface {
 public:
     virtual ~LexiconInterface() {}

     // avoid public virtual functions
     // I'm blindly following <http://www.gotw.ca/publications/mill18.htm> here
     void init() {
         do_init();
     }
     void init(const std::map<std::string, std::string>& params) {
         do_set_from_params(params);
         do_init();
     }
     void clear() {
         _entries_cache_initialized = false;
         _entries_cache.clear();
         do_clear();
     }
     void set_from_params(const std::map<std::string, std::string>& params) {
         do_set_from_params(params);
     }
     void save_params() {
         do_save_params();
     }
     bool contains(const string_impl& word) const {
         return check_contains(word);
     }
     bool contains_partial(const string_impl& word) const {
         return check_contains_partial(word);
     }
     void add(const string_impl& word) {
         bool added = add_word(word);
         if (added && _entries_cache_initialized)
             _entries_cache.push_back(word);
     }
     std::vector<string_impl> entries() const {
         if (!_entries_cache_initialized) {
             _entries_cache = retrieve_all_entries();
             _entries_cache_initialized = true;
         }
         return _entries_cache;
     }
     unsigned int size() const {
         return get_size();
     }

 protected:
     LexiconInterface() = default;

 private:
     virtual void do_init() = 0;
     virtual void do_clear() = 0;
     virtual void do_set_from_params(const std::map<std::string, std::string>&
                                     params) = 0;
     virtual void do_save_params() = 0;
     virtual bool check_contains(const string_impl& word) const = 0;
     virtual bool check_contains_partial(const string_impl& word) const = 0;
     virtual bool add_word(const string_impl& word) = 0;
     virtual std::vector<string_impl> retrieve_all_entries() const = 0;
     virtual unsigned int get_size() const = 0;

     mutable std::vector<string_impl> _entries_cache;
     mutable bool _entries_cache_initialized = false;
};

}  // namespace Normalizer
}  // namespace Norma

#endif  // NORMALIZER_LEXICON_INTERFACE_H_
