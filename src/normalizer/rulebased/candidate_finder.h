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
#ifndef NORMALIZER_RULEBASED_CANDIDATE_FINDER_H_
#define NORMALIZER_RULEBASED_CANDIDATE_FINDER_H_
#include<vector>
#include<map>
#include<queue>
#include<functional>
#include"string_impl.h"
#include"symbols.h"
#include"result.h"
#include"rule.h"

namespace Norma {
namespace Normalizer {
class LexiconInterface;
namespace Rulebased {
class RuleCollection;

/// Stores the state of a normalization step
struct RAState {
    double fscore;              // theoretical minimum cost until end of word
    double cost;                // current cost at this state
    string_size pos;            // current position in the word
    bool epsilon;               // if true, we're in the epsilon slot before
                                // the position
    string_impl norm;           // normalization generated so far
    string_impl _word;
    std::vector<Rule> history;  // rules applied in this path

    string_impl left() const {
        return (is_empty(norm) ? from_char(Symbols::BOUNDARY) : norm);
    }

    explicit RAState(const string_impl& word)
        : fscore(0.0), cost(0.0), pos(0), epsilon(true), norm(""), _word(word)
        {}
    RAState(double f, double c, unsigned int p, bool e, string_impl n,
            std::vector<Rule> h) : fscore(f), cost(c), pos(p), epsilon(e),
                                   norm(n), history(h) {}
    bool end_of_word() const {
        return (pos >= _word.length() && !epsilon);
    }

    // for sorting states in a priority queue
    bool operator>(const RAState& that) const {
        return fscore > that.fscore;
    }
};

class Rulebased;

class CandidateFinder {
 public:
    CandidateFinder() = delete;
    CandidateFinder(const string_impl& word,
                    const RuleCollection& rules,
                    const LexiconInterface& lex);
    Result operator()();

 private:
    void iterate_over_rules(const RAState& current,
                            const std::vector<Rule>& applicable_rules);
    double calculate_rule_cost(const Rule& r) const;
    int calculate_combined_frequency(const std::vector<Rule>&
                                     applicable_rules) const;
    double estimate_cost(const int pos, bool eps) {
        return (_total_steps - 2 * pos - (eps ? 1: 0));
    }
    double cost_to_probability(const double cost);
    const RuleCollection* _rules;
    const LexiconInterface* _lex;
    string_impl word_bound;
    Result unchanged_result;
    int _total_steps;
    int _minimum_combined_frequency;
    std::priority_queue<RAState, std::vector<RAState>,
                        std::greater<RAState>> _q;
    std::map<std::tuple<int, bool, string_impl>, double> best_fscore;
};

}  // namespace Rulebased
}  // namespace Normalizer
}  // namespace Norma

#endif  // NORMALIZER_RULEBASED_CANDIDATE_FINDER_H_

