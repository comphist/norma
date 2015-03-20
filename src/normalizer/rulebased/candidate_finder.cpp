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
#include"candidate_finder.h"
#include<algorithm>
#include<sstream>
#include<vector>
#include"symbols.h"
#include"lexicon.h"
#include"result.h"
#include"rule.h"
#include"rule_collection.h"

namespace Norma {
namespace Normalizer {
namespace Rulebased {

CandidateFinder::CandidateFinder(const string_impl& word,
                                 const RuleCollection& rules,
                                 const LexiconInterface& lex)
    : _rules(&rules), _lex(&lex) {
    word_bound = word + Symbols::BOUNDARY;
    unchanged_result = Result(word, 0.0);
    unchanged_result.messages.push(make_message(LogLevel::TRACE, "RuleBased",
                                                "no candidate found"));
    _total_steps = (2 * word.length()) + 1;
    // this is experimental -- not clear what the best setting is:
    _minimum_combined_frequency = 2 * _rules->get_average_freq();
    _q.push(RAState(word));
}

Result CandidateFinder::operator()() {
    while (!_q.empty()) {
        const RAState current = _q.top();
        _q.pop();
        if (current.end_of_word()) {
            if (!_lex->contains(current.norm))
                continue;
            else {  // success!
                Result result = Result(current.norm, cost_to_probability(current.cost));
                for (const Rule& rule : current.history) {
                    std::ostringstream message;
                    message << "applied rule: " << rule;
                    result.messages.push(make_message(LogLevel::TRACE, "RuleBased",
                                                      message.str()));
                }
                return result;
            }
        }

        string_impl current_back;
        extract(word_bound, current.pos, word_bound.length(), &current_back);
        iterate_over_rules(current,
                           _rules->find_applicable_rules(current.left(),
                                                         current_back,
                                                         current.epsilon));
    }
    // TODO(mbollmann): if applicable_rules is empty, allow the identity
    //       rule as fallback?
    //       -- this is what the old rules applicator did
    return unchanged_result;  // failure!
}

void CandidateFinder::iterate_over_rules(const RAState& current,
                                         const std::vector<Rule>&
                                               applicable_rules) {
    int combined_freq = calculate_combined_frequency(applicable_rules);
    for (const Rule& rule : applicable_rules) {
        RAState next = current;
        if (rule.to() != Symbols::EPSILON)
            next.norm += rule.to();
        if (!_lex->contains_partial(next.norm))
            continue;
        if (!current.epsilon)
            next.pos += rule.from().length();
        next.epsilon = !current.epsilon;
        next.cost += calculate_rule_cost(rule) * combined_freq;
        next.fscore = next.cost + estimate_cost(next.pos, next.epsilon);
        next.history.push_back(rule);
        auto best_fscore_hash =
            std::make_tuple(next.pos, next.epsilon, next.norm);
        if (best_fscore.count(best_fscore_hash) > 0
         && best_fscore[best_fscore_hash] <= next.fscore)
            continue;
        best_fscore[best_fscore_hash] = next.fscore;
        _q.push(next);
    }
}

int CandidateFinder::calculate_combined_frequency(const std::vector<Rule>&
                                                  applicable_rules) const {
    int combined_freq = 0;
    for (const Rule& rule : applicable_rules) {
        combined_freq += _rules->get_freq(rule);
    }
    return std::max(_minimum_combined_frequency, combined_freq);
}

double CandidateFinder::calculate_rule_cost(const Rule& r) const {
    auto from_len = r.from().length();
    if (from_len > 1) {
        return static_cast<double>((2 * from_len) - 1) / _rules->get_freq(r);
    }
    return 1.0 / _rules->get_freq(r);
}

double CandidateFinder::cost_to_probability(const double cost) {
    return std::min(1.0, _total_steps / cost);
}


}  // namespace Rulebased
}  // namespace Normalizer
}  // namespace Norma
