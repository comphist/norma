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
#ifndef NORMALIZER_RULEBASED_RULE_COLLECTION_H_
#define NORMALIZER_RULEBASED_RULE_COLLECTION_H_
#include<unordered_map>
#include<tuple>
#include<string>
#include<vector>
#include"regex_impl.h"
#include"rule.h"

namespace Norma {
namespace Normalizer {
namespace Rulebased {

struct RAState;

/// A collection of rewrite rules as a parametrization for the
/// rule-based normalizer.  Encapsulates functions related to
/// the retrieval, storage, and cost calculation of rewrite rules.
class RuleCollection {
 public:
     RuleCollection() { _rule_re = make_regex_impl(_rule_re_str); }
     void clear();
     bool read_rulesfile(const std::string& fname);
     bool save_rulesfile(const std::string& fname);

     void learn_rule(Rule r, int count = 1);
     void learn_ruleset(const RuleSet& rs);

     std::vector<Rule> find_applicable_rules(const string_impl& left,
                                             const string_impl& back,
                                             bool epsilon) const;

     int get_freq(const Rule& r) const;
     int get_highest_freq() const { return _highest_freq; }
     int get_type_count() const { return _rules.size(); }
     int get_instance_count() const { return _total_count; }
     int get_average_freq() const {
         if (_rules.empty()) return 0;
         return _total_count / _rules.size();
     }

 private:
     // input/output of rules files
     const std::string _rule_re_str
         = "[^0-9]*([0-9]+)\\s+\\{(.+)->(.+)/(.)_(.)\\}.*$";
     regex_impl _rule_re;
     std::tuple<Rule, int> parse_line(const std::string& line);

     // data members
     std::unordered_map<Rule, int, RuleHasher> _rules;
     int _total_count = 0;   // counts rule instances, not types
     int _highest_freq = 0;  // max(#instances)
};

}  // namespace Rulebased
}  // namespace Normalizer
}  // namespace Norma

#endif  // NORMALIZER_RULEBASED_RULE_COLLECTION_H_
