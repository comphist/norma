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
#include"levenshtein_algorithm.h"
#include<algorithm>
#include<vector>
#include"typedefs.h"
#include"weight_set.h"
#include"gfsm_wrapper.h"
#include"string_impl.h"

using std::vector;

namespace Norma {
namespace Normalizer {
namespace WLD {
namespace {
inline RuleSet copy_push_back(const RuleSet& rs, const EditPair& ep) {
    RuleSet new_rs(rs); new_rs.push_back(ep); return new_rs;
}

typedef vector<string_impl>::size_type vec_size;

class WldLogic {
 protected:
     // set from the ctor
     const vector<string_impl>& _source, & _target;
     const WeightSet& ws;

     // calculated (cached) by the ctor
     vec_size source_len, target_len;

     // calculated in the methods and held here for following methods
     EditPair ins_pair, del_pair, sub_pair;
     std::vector<double> this_row, next_row;
     double best_cost, ins_cost, del_cost, sub_cost;

 public:
     WldLogic(const vector<string_impl>& source,
              const vector<string_impl>& target,
              const WeightSet& weights)
         : _source(source), _target(target), ws(weights),
           source_len(source.size()), target_len(target.size()),
           this_row(target_len+1), next_row(target_len+1) {}

     virtual vec_size slen() { return source_len; }
     virtual vec_size tlen() { return target_len; }

     // called once at the start of the main loop, it initializes
     // the top left cell and the top row
     virtual void init_toprow() {
         this_row[0] = 0;
         for (vec_size tpos = 0; tpos < target_len; ++tpos)
             this_row[tpos+1] = this_row[tpos]
                 + ws.get_weight(EditPair({}, {_target[tpos]}));
     }
     // called once _before_ every inner (target) loop,
     // it initializes the left column of the next row
     virtual void init_leftcolumn(vec_size spos) {
         next_row[0] = this_row[0]
             + ws.get_weight(EditPair({_source[spos]}, {}));
     }
     // called on every iteration of the inner main loop,
     // it fills the rest of the next row
     virtual void main_body(vec_size spos, vec_size tpos) {
         ins_pair = EditPair({}, {_target[tpos]});
         del_pair = EditPair({_source[spos]}, {});
         sub_pair = EditPair({_source[spos]}, {_target[tpos]});
         ins_cost = next_row[tpos] + ws.get_weight(ins_pair);
         del_cost = this_row[tpos+1] + ws.get_weight(del_pair);
         sub_cost = this_row[tpos] + ws.get_weight(sub_pair);
         best_cost = std::min({ins_cost, del_cost, sub_cost});
         next_row[tpos+1] = best_cost;
     }
     // called once _after_ every inner (target) loop,
     // it swaps the next row into this row
     virtual void make_swap() {
         this_row.swap(next_row);
     }
     // return the result. this can't be virtual since the results
     // have different types, but it doesn't need to be either since
     // it will called on a ptr to base
     double get_result() {
         return this_row[target_len];
     }
};

// for documentation on the methods, see the base class above
class AlignLogic : public WldLogic {
 private:
     vector<AlignmentSet> this_edit, next_edit;

 public:
     AlignLogic(const vector<string_impl>& source,
                const vector<string_impl>& target,
                const WeightSet& weights)
         : WldLogic(source, target, weights),
           this_edit(target_len+1), next_edit(target_len+1) {}
     void init_toprow() {
         WldLogic::init_toprow();
         this_edit[0] = {{}};
         for (vec_size tpos = 0; tpos < target_len; ++tpos) {
             EditPair p({}, {_target[tpos]});
             this_edit[tpos+1] = { copy_push_back(this_edit[tpos][0], p) };
         }
     }
     void init_leftcolumn(vec_size spos) {
         WldLogic::init_leftcolumn(spos);
         next_edit[0].push_back(copy_push_back(this_edit[0][0],
                                EditPair({_source[spos]}, {})));
     }
     void main_body(vec_size spos, vec_size tpos) {
         WldLogic::main_body(spos, tpos);
         auto update_edit = [&](AlignmentSet s, EditPair p) {
             for (const auto& rs : s)
                 next_edit[tpos+1].push_back(copy_push_back(rs, p));
         };  // NOLINT[readability/braces]
         // the lint target seems to ignore the previous NOLINT, idk why
         next_edit[tpos+1] = {};
         if (ins_cost <= best_cost)
             update_edit(next_edit[tpos], ins_pair);
         if (del_cost <= best_cost)
             update_edit(this_edit[tpos+1], del_pair);
         if (sub_cost <= best_cost)
             update_edit(this_edit[tpos], sub_pair);
     }
     void make_swap() {
         WldLogic::make_swap();
         this_edit.swap(next_edit);
     }
     AlignmentSet get_result() {
         return this_edit[target_len];
     }
};

// basically just a double nested loop that calls the appropriate
// methods of the logic object
void main_loop(WldLogic* logic) {
    logic->init_toprow();

    for (vec_size spos = 0; spos < logic->slen(); ++spos) {
        logic->init_leftcolumn(spos);
        for (vec_size tpos = 0; tpos < logic->tlen(); ++tpos)
            logic->main_body(spos, tpos);
        logic->make_swap();
    }
}

}  // namespace

AlignmentSet align(const string_impl& from, const string_impl& to,
                   const WeightSet& weights) {
    auto source = Gfsm::explode(from),
         target = Gfsm::explode(to);
    return align(source, target, weights);
}

AlignmentSet align(const WordPair& p, const WeightSet& weights) {
    return align(p.first, p.second, weights);
}

AlignmentSet align(const std::vector<string_impl>& source,
                   const std::vector<string_impl>& target,
                   const WeightSet& weights) {
    AlignLogic logic(source, target, weights);
    main_loop(&logic);
    return logic.get_result();
}

double wld(const string_impl& from, const string_impl& to,
           const WeightSet& weights) {
    auto source = Gfsm::explode(from),
         target = Gfsm::explode(to);
    return wld(source, target, weights);
}

double wld(const WordPair& p, const WeightSet& weights) {
    return wld(p.first, p.second, weights);
}

double wld(const EditPair& p, const WeightSet& weights) {
    return wld(p.first, p.second, weights);
}

double wld(const std::vector<string_impl>& source,
           const std::vector<string_impl>& target,
           const WeightSet& weights) {
    WldLogic logic(source, target, weights);
    main_loop(&logic);
    return logic.get_result();
}
}  // namespace WLD
}  // namespace Normalizer
}  // namespace Norma

