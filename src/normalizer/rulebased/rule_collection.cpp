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
#include"rule_collection.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include"normalizer/exceptions.h"
#include"regex_impl.h"
#include"string_impl.h"
#include"symbols.h"

namespace Norma {
namespace Normalizer {
namespace Rulebased {

void RuleCollection::clear() {
    _rules.clear();
    _total_count = 0;
    _highest_freq = 0;
}

void RuleCollection::learn_rule(Rule r, int count) {
    if (_rules.count(r) > 0) {
        _rules.at(r) += count;
    } else {
        _rules.insert(std::make_pair(r, count));
    }
    // new highest frequency?
    int& new_count = _rules.at(r);
    if (new_count > _highest_freq) {
        _highest_freq = new_count;
    }
    // increase instance count
    _total_count += count;
}

void RuleCollection::learn_ruleset(const RuleSet& rs) {
    for (auto rule = rs.cbegin(); rule != rs.cend(); rule++) {
        learn_rule(*rule, 1);
    }
}

std::vector<Rule> RuleCollection::find_applicable_rules(const string_impl& left,
                                                        const string_impl& back,
                                                        bool epsilon) const {
    std::vector<Rule> applicable_rules;
    for (auto& elem : _rules) {
        const Rule& rule = elem.first;
        if (rule.matches_left(left[left.length() - 1])
         && rule.matches_back(back, epsilon)) {
            applicable_rules.push_back(rule);
        }
    }
    return applicable_rules;
}

int RuleCollection::get_freq(const Rule& r) const {
    if (_rules.count(r) > 0) {
        return _rules.at(r);
    }
    return 0;
}

std::tuple<Rule, int> RuleCollection::parse_line(const std::string& line) {
    boost::smatch what;
    Rule r;
    if (REGEX_IMPL_MATCH(line, what, _rule_re, boost::match_extra)) {
        r = Rule(what[2].str().c_str(), what[3].str().c_str(),
                 what[4].str().c_str(), what[5].str().c_str());
        std::istringstream i(what[1]);
        int count;
        if (i >> count) {
            return std::make_tuple(r, count);
        }
    }
    return std::make_tuple(r, 0);
}

bool RuleCollection::read_rulesfile(const std::string& fname) {
    std::ifstream file;
    file.open(fname);
    if (!file.is_open())
        throw init_error("couldn't open parameter file: " + fname);
    std::string line = "";
    int invalid_line_count = 0;
    while (getline(file, line)) {
        if (file.eof())
            break;
        Rule r;
        int count;
        std::tie(r, count) = parse_line(line);
        if (count != 0)
            learn_rule(r, count);
        else  // line empty or could not be parsed
            ++invalid_line_count;
    }
    file.close();
    if (invalid_line_count > 1) {
        std::ostringstream msg;
        msg << "couldn't parse " << invalid_line_count
            << " lines in parameter file: " << fname;
        throw init_error(msg.str());
    }
    return true;
}

bool RuleCollection::save_rulesfile(const std::string& fname) {
    std::ofstream file;
    file.open(fname);
    if (!file.is_open())
        return false;
    for (auto& elem : _rules) {
        file << elem.second << "  " << elem.first << std::endl;
    }
    file.close();
    return true;
}

}  // namespace Rulebased
}  // namespace Normalizer
}  // namespace Norma
