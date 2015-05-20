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
#include"string_impl.h"
#include"normalizer/base.h"
#include"normalizer/cacheable.h"
#include"normalizer/result.h"
#include"lexicon/lexicon.h"
#include"rule_collection.h"
#include"rule_learn.h"
#include"candidate_finder.h"

namespace Norma {
namespace Normalizer {
namespace Rulebased {

class Rulebased : public Base, public Cacheable {
 public:
     void init();
     using Base::init;
     void set_from_params(const std::map<std::string, std::string>& params);
     void clear();

     /// Get filename of the current rules file
     const std::string& get_rulesfile() const { return _rulesfile; }
     /// Set filename for the rules file
     Rulebased& set_rulesfile(const std::string& rulesfile) {
         _rulesfile = rulesfile;
         return *this;
     }

     using Cacheable::set_caching;
     using Cacheable::clear_cache;
     using Cacheable::is_caching;

 protected:
     bool do_train(TrainingData* data);
     Result do_normalize(const string_impl& word) const;
     ResultSet do_normalize(const string_impl& word, unsigned int n) const;
     void do_save_params();

 private:
     std::string _rulesfile;
     RuleCollection _rules;
};
}  // namespace Rulebased
}  // namespace Normalizer
}  // namespace Norma

extern "C" Norma::Normalizer::Base* create_normalizer() {
    return new Norma::Normalizer::Rulebased::Rulebased;
}
extern "C" void destroy_normalizer(Norma::Normalizer::Base* n) {
    delete n;
}

#endif  // NORMALIZER_RULEBASED_RULEBASED_H_

