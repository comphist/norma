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
#ifndef NORMALIZER_WLD_LEVENSHTEIN_ALIGNER_H_
#define NORMALIZER_WLD_LEVENSHTEIN_ALIGNER_H_
#include<map>
#include<set>
#include<tuple>
#include<vector>
#include"string_impl.h"
#include"typedefs.h"
#include"weight_set.h"

namespace Norma {
namespace Normalizer {
namespace WLD {
class WeightSet;

class LevenshteinAligner {
 public:
    LevenshteinAligner(const WeightSet& ws,
                       unsigned int n = 3, unsigned int d = 7)
        : _weights(ws), _ngrams(n), _divisor(d) {}

    // Performs a PMI training cycle, updating the WeightSet
    void perform_training_cycle(const TrainSet& pairs);
    WeightSet make_final_weight_set(const TrainSet& pairs);

    WeightSet& weight_set() { return _weights; }
    const WeightSet& weight_set() const { return _weights; }
    double& learning_rate() { return _learning_rate; }
    const double& learning_rate() const { return _learning_rate; }
    bool& allow_pure_insertions() { return _allow_pure_insertions; }
    const bool& allow_pure_insertions() const { return _allow_pure_insertions; }
    bool& allow_identity() { return _allow_identity; }
    const bool& allow_identity() const { return _allow_identity; }
    double meandiff() const { return _meandiff; }

 private:
    struct RuleStats {
        int freq = 0;
        double pmi = 0.0;
    };
    typedef std::map<EditPair, RuleStats> RuleStatsMap;
    typedef std::map<std::vector<string_impl>, int> NgramFrequencyMap;
    typedef std::map<std::vector<string_impl>,
                     std::set<std::vector<string_impl>>> PairTypesMap;

    WeightSet _weights;
    unsigned int _ngrams;
    unsigned int _divisor;
    double _learning_rate = 0.2;
    double _meandiff = 0;
    bool _allow_pure_insertions = false;
    bool _allow_identity = false;

    void collect_unigram_frequencies(RuleStatsMap* fr,
                                     NgramFrequencyMap* fs,
                                     NgramFrequencyMap* ft,
                                     const AlignmentSet& as,
                                     int count = 1) const;
    void collect_frequencies(RuleStatsMap* fr,
                             NgramFrequencyMap* fs, PairTypesMap* pt,
                             const AlignmentSet& as, int count = 1) const;
    std::tuple<double, double> calculate_pmi(RuleStatsMap* rules,
                                             const NgramFrequencyMap& fs,
                                             const NgramFrequencyMap& ft) const;
    double adjust_weights(const RuleStatsMap& rules,
                          std::tuple<double, double> pmi);
};
}  // namespace WLD
}  // namespace Normalizer
}  // namespace Norma

#endif  // NORMALIZER_WLD_LEVENSHTEIN_ALIGNER_H_
