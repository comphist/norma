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
#include"levenshtein_aligner.h"
#include<algorithm>
#include<cmath>
#include<limits>
#include<utility>
#include<vector>
#include"gfsm_wrapper.h"
#include"string_impl.h"
#include"symbols.h"
#include"typedefs.h"
#include"levenshtein_algorithm.h"
#include"weight_set.h"

namespace Norma {
namespace Normalizer {
namespace WLD {
void LevenshteinAligner::perform_training_cycle(const TrainSet& pairs) {
    RuleStatsMap rules;
    NgramFrequencyMap freq_source, freq_target;
    // calculate unigram alignments & collect rule frequencies
    for (const auto& elem : pairs) {
        collect_unigram_frequencies(&rules, &freq_source, &freq_target,
                                    align(elem.first, _weights), elem.second);
    }
    // pointwise mutual information
    auto pmi = calculate_pmi(&rules, freq_source, freq_target);
    // adjust weights
    _meandiff = adjust_weights(rules, pmi);
}

WeightSet LevenshteinAligner::make_final_weight_set(const TrainSet& pairs) {
    RuleStatsMap rules;
    NgramFrequencyMap freq_source;
    PairTypesMap targets_per_source;
    WeightSet final;
    int pair_count = 0;
    // calculate final n-gram alignments & collect frequencies
    for (const auto& elem : pairs) {
        collect_frequencies(&rules, &freq_source, &targets_per_source,
                            align(elem.first, _weights), elem.second);
        pair_count += elem.second;
    }
    // calculate final weights
    double alpha = 0.5;
    int freq_floor = pair_count / 6.293;
    for (const auto& rule : rules) {
        const EditPair& pair = rule.first;
        const RuleStats& stats = rule.second;
        if (!_allow_pure_insertions && pair.first.empty())
            continue;
        if (!_allow_identity && pair.first == pair.second)
            continue;
        double prob = (static_cast<double>(stats.freq) + alpha)
                      / (std::max(freq_floor, freq_source[pair.first])
                         + (alpha * targets_per_source[pair.first].size()));
        final.add_weight(pair, -std::log(prob) / _divisor);
    }
    return final;
}

void LevenshteinAligner::collect_unigram_frequencies(RuleStatsMap* fr,
                                                     NgramFrequencyMap* fs,
                                                     NgramFrequencyMap* ft,
                                                     const AlignmentSet& as,
                                                     int count) const {
    for (const RuleSet& rs : as) {
        for (auto it = rs.begin(); it != rs.end(); ++it) {
            EditPair ep;
            (*fr)[ep].freq += count;
            if ((*fs).count(ep.first) == 0)
                (*fs)[ep.first]  = count;
            else
                (*fs)[ep.first] += count;
            if ((*ft).count(ep.second) == 0)
                (*ft)[ep.second]  = count;
            else
                (*ft)[ep.second] += count;
        }
    }
}

void LevenshteinAligner::collect_frequencies(RuleStatsMap* fr,
                                             NgramFrequencyMap* fs,
                                             PairTypesMap* pt,
                                             const AlignmentSet& as,
                                             int count) const {
    for (const RuleSet& rs : as) {
        for (auto it = rs.begin(); it != rs.end(); ++it) {
            EditPair ep;
            unsigned int n = 0;
            for (auto jt = it; jt != rs.end() && n < _ngrams; ++jt, ++n) {
                ep.first.insert(ep.first.end(),
                                (*jt).first.begin(), (*jt).first.end());
                ep.second.insert(ep.second.end(),
                                 (*jt).second.begin(), (*jt).second.end());
                (*fr)[ep].freq += count;
                if ((*fs).count(ep.first) == 0)
                    (*fs)[ep.first]  = count;
                else
                    (*fs)[ep.first] += count;
                (*pt)[ep.first].insert(ep.second);
            }
        }
    }
}

std::tuple<double, double>
LevenshteinAligner::calculate_pmi(RuleStatsMap* rules,
                                  const NgramFrequencyMap& fs,
                                  const NgramFrequencyMap& ft) const {
    double max_pmi = 0.0, min_pmi = std::numeric_limits<double>::max();
    for (auto& rule : *rules) {
        const EditPair& pair = rule.first;
        RuleStats& stats = rule.second;
        stats.pmi = std::log2((static_cast<double>(stats.freq)
                               / fs.at(pair.first)) / ft.at(pair.second));
        if (stats.pmi > max_pmi)
            max_pmi = stats.pmi;
        if (stats.pmi < min_pmi)
            min_pmi = stats.pmi;
    }
    return std::make_tuple(min_pmi, max_pmi);
}

double LevenshteinAligner::adjust_weights(const RuleStatsMap& rules,
                                          std::tuple<double, double> pmi) {
    WeightSet new_weights;
    new_weights.copy_defaults(_weights);
    double min_pmi = std::get<0>(pmi), max_pmi = std::get<1>(pmi);
    double range_pmi = max_pmi - min_pmi;
    double mean_diff = 0.0;
    for (auto& rule : rules) {
        const EditPair& pair = rule.first;
        const RuleStats& stats = rule.second;
        if (!_allow_pure_insertions && pair.first.empty())
            continue;
        if (!_allow_identity && pair.first == pair.second)
            continue;
        double dist = (max_pmi - stats.pmi) / range_pmi;
        double w_old = _weights.get_weight(pair);
        double w_new = (w_old * (1.0 - _learning_rate))
                       + (_learning_rate * dist);
        new_weights.add_weight(pair, w_new);
        mean_diff += std::abs(w_new - w_old);
    }
    mean_diff /= rules.size();
    _weights = new_weights;
    return mean_diff;
}
}  // namespace WLD
}  // namespace Normalizer
}  // namespace Norma

