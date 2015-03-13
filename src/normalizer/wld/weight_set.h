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
#ifndef NORMALIZER_WLD_WEIGHT_SET_H_
#define NORMALIZER_WLD_WEIGHT_SET_H_
#include<map>
#include<set>
#include<string>
#include<tuple>
#include<vector>
#include"gfsm_wrapper.h"
#include"string_impl.h"
#include"typedefs.h"
#include"symbols.h"

namespace Norma {
namespace Normalizer {
namespace WLD {
class WeightSet {
 public:
     void clear();
     bool read_paramfile(const std::string& fname);
     bool save_paramfile(const std::string& fname);

     const std::set<string_impl>& input_symbols() const
         { return _input_symbols; }
     const std::vector<Gfsm::StringPath> weights() const;
     const std::map<EditPair, double>& weight_map() const
         { return _weights; }
     size_t size() const { return _weights.size(); }
     bool empty() const { return _weights.empty(); }
     double& default_identity_cost()
         { return _default_identity_cost; }
     double& default_replacement_cost()
         { return _default_replacement_cost; }
     double& default_insertion_cost()
         { return _default_insertion_cost; }
     double& default_deletion_cost()
         { return _default_deletion_cost; }
     const double& default_identity_cost() const
         { return _default_identity_cost; }
     const double& default_replacement_cost() const
         { return _default_replacement_cost; }
     const double& default_insertion_cost() const
         { return _default_insertion_cost; }
     const double& default_deletion_cost() const
         { return _default_deletion_cost; }

     void copy_defaults(const WeightSet& ws);
     void add_weight(const string_impl& from, const string_impl& to,
                     double weight);
     void add_weight(const EditPair& edit, double weight);
     double get_weight(const string_impl& from, const string_impl& to) const;
     double get_weight(const EditPair& pair) const;
     void divide_all(double divisor);

 private:
     double _default_identity_cost = 0.0,
         _default_replacement_cost = 1.0,
         _default_insertion_cost = 1.0,
         _default_deletion_cost = 1.0;

     /// Custom weights
     std::map<EditPair, double> _weights;

     /// Set of all used input symbols
     std::set<string_impl> _input_symbols;

     double calculate_wld(const EditPair& pair) const;
     static EditPair make_editpair(const string_impl& from,
                                   const string_impl& to);
};
}  // namespace WLD
}  // namespace Normalizer
}  // namespace Norma

#endif  // NORMALIZER_WLD_WEIGHT_SET_H_
