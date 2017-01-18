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
#include"rule_learn.h"
#include<list>
#include<vector>
#include<stdexcept>

namespace Norma {
namespace Normalizer {
namespace Rulebased {
/////////////////////// local helper functions ////////////////////////////////
namespace {

void construct_initial_matrix(std::vector<std::vector<RuleSet>> *matrix,
                              const string_impl& source,
                              const string_impl& target) {
    int n = source.length(),
        m = target.length();
    matrix->resize(n + 1);
    for (int i = 0; i <= n; ++i)
        (*matrix)[i].resize(m + 1);

    // top row
    RuleSet edits;
    for (int p = 0; p < m; ++p) {
        edits.add_rule(EditOp::ADD, source, 1, target, p + 1);
        (*matrix)[0][p + 1] = RuleSet(edits);
    }

    // left col
    edits = RuleSet();
    for (int p = 0; p < n; ++p) {
        edits.add_rule(EditOp::DEL, source, p + 1, target, 1);
        (*matrix)[p + 1][0] = RuleSet(edits);
    }
}
}  // namespace

RuleSet learn_rules(const string_impl& source, const string_impl& target,
                    bool do_merge = true, bool insert_epsilon = true) {
    int n = source.length(),
        m = target.length();

    std::vector<std::vector<RuleSet>> _matrix;
    construct_initial_matrix(&_matrix, source, target);

    RuleSet edits;
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j) {
            int sc = (source[i - 1] == target[j - 1]) ? 0 : 1,
                add_cost = _matrix[i][j-1].cost() + 1,
                del_cost = _matrix[i-1][j].cost() + 1,
                sub_cost = _matrix[i-1][j-1].cost() + sc;
            if (sub_cost <= add_cost && sub_cost <= del_cost) {
                edits = RuleSet(_matrix[i-1][j-1]);
                edits.add_rule(EditOp::SUB, source, i, target, j);
            } else if (del_cost <= sub_cost && del_cost <= add_cost) {
                edits = RuleSet(_matrix[i-1][j]);
                edits.add_rule(EditOp::DEL, source, i, target, j);
            } else if (add_cost <= sub_cost && add_cost <= del_cost) {
                edits = RuleSet(_matrix[i][j-1]);
                edits.add_rule(EditOp::ADD, source, i, target, j);
            }
            _matrix[i][j] = edits;
        }

    edits = _matrix.back().back();

    if (do_merge)
        edits.merge_rules();

    if (insert_epsilon)
        edits.insert_epsilon_identity(source, target);

    return edits;
}

}  // namespace Rulebased
}  // namespace Normalizer
}  // namespace Norma
