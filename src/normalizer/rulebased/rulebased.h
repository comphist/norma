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
#ifndef NORMALIZER_RULEBASED_RULEBASED_H_
#define NORMALIZER_RULEBASED_RULEBASED_H_
#include<map>
#include<string>
#include<mutex>
#include<memory>
#include"string_impl.h"
#include"base.h"
#include"result.h"
#include"rule_collection.h"
#include"rule_learn.h"
#include"lexicon/lexicon.h"
#include"candidate_finder.h"

namespace Norma {
namespace Normalizer {
namespace Rulebased {

class Rulebased : public Base {
 public:
     Rulebased();
     void init();
     using Base::init;
     void set_from_params(const std::map<std::string, std::string>& params);
     void clear();
     Result operator()(const string_impl& word) const;
     ResultSet operator()(const string_impl& word, unsigned int n) const;
     bool train(TrainingData* data);
     void save_params();
     const char* name() const { return "RuleBased"; }

     /// Get filename of the current rules file
     const std::string& get_rulesfile() const { return _rulesfile; }
     /// Set filename for the rules file
     Rulebased& set_rulesfile(const std::string& rulesfile) {
         _rulesfile = rulesfile;
         return *this;
     }

     void set_caching(bool value) const;
     void clear_cache() const;
     bool is_caching() const { return _caching; }

 private:
     std::string _rulesfile;
     RuleCollection _rules;

     mutable bool _caching = true;
     mutable std::map<string_impl, Result> _cache;
     mutable std::unique_ptr<std::mutex> cache_mutex;
};
}  // namespace Rulebased
}  // namespace Normalizer
}  // namespace Norma

#endif  // NORMALIZER_RULEBASED_RULEBASED_H_

