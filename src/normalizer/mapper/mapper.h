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
#ifndef NORMALIZER_MAPPER_MAPPER_H_
#define NORMALIZER_MAPPER_MAPPER_H_
#include<map>
#include<string>
#include<fstream>
#include<tuple>
#include"string_impl.h"
#include"normalizer/base.h"
#include"normalizer/result.h"

namespace Norma {
namespace Normalizer {
class Mapper : public Base {
 public:
     void init();
     using Base::init;
     void set_from_params(const std::map<std::string, std::string>& params);
     void clear();
     Result operator()(const string_impl& word) const;
     ResultSet operator()(const string_impl& word, unsigned int n) const;
     bool train(TrainingData* data);
     void train(const string_impl& word, const string_impl& modern,
                int count);
     void save_params();
     const char* name() const { return "Mapper"; }

     /// Get filename of the current mappings file
     const std::string& get_mapfile() const { return _mapfile; }
     /// Set filename for the mappings file
     Mapper& set_mapfile(const std::string& mapfile) {
         _mapfile = mapfile;
         return *this;
     }

 private:
     ResultSet make_all_results(const string_impl& word) const;
     bool write_mapfile(const std::string& fname);
     bool read_mapfile(const std::string& fname);

     std::map<string_impl, std::map<string_impl, int>> _map;
     std::string _mapfile;
};
}  // namespace Normalizer
}  // namespace Norma

#endif  // NORMALIZER_MAPPER_H_

