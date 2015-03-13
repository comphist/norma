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
#include"weight_set.h"
#include<algorithm>
#include<fstream>
#include<sstream>
#include<string>
#include<tuple>
#include<vector>
#include"exceptions.h"
#include"gfsm_wrapper.h"
#include"string_impl.h"
#include"levenshtein_algorithm.h"
#include"symbols.h"

namespace Norma {
namespace Normalizer {
namespace WLD {
void WeightSet::clear() {
    _input_symbols.clear();
    _weights.clear();
}

void WeightSet::copy_defaults(const WeightSet& ws) {
    _default_identity_cost = ws.default_identity_cost();
    _default_replacement_cost = ws.default_replacement_cost();
    _default_insertion_cost = ws.default_insertion_cost();
    _default_deletion_cost = ws.default_deletion_cost();
}

const std::vector<Gfsm::StringPath> WeightSet::weights() const {
    std::vector<Gfsm::StringPath> paths;
    for (auto& elem : _weights) {
        Gfsm::StringPath path(elem.first.first, elem.first.second, elem.second);
        paths.push_back(path);
    }
    return paths;
}

bool WeightSet::read_paramfile(const std::string& fname) {
    std::ifstream file;
    file.open(fname);
    if (!file.is_open())
        throw init_error("couldn't open parameter file: " + fname);
    std::string line = "";
    int invalid_line_count = 0;
    while (getline(file, line)) {
        if (file.eof())
            break;
        std::istringstream iss(line);
        string_impl source, target;
        double weight;
        iss >> source >> target >> weight;
        if (iss)
            add_weight(source, target, weight);
        else
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

bool WeightSet::save_paramfile(const std::string& fname) {
    std::ofstream file;
    file.open(fname);
    if (!file.is_open())
        return false;
    for (const auto& elem : _weights) {
        string_impl first = elem.first.first.empty() ?
            Symbols::EPS : Gfsm::implode(elem.first.first);
        string_impl second = elem.first.second.empty() ?
            Symbols::EPS : Gfsm::implode(elem.first.second);
        file << first << "\t" << second << "\t"
             << elem.second << std::endl;
    }
    file.close();
    return true;
}

EditPair WeightSet::make_editpair(const string_impl& from,
                                  const string_impl& to) {
    std::vector<string_impl> from_vec, to_vec;
    if (from != Symbols::EPS) from_vec = Gfsm::explode(from);
    if (to != Symbols::EPS)   to_vec   = Gfsm::explode(to);
    return std::make_pair(from_vec, to_vec);
}

void WeightSet::add_weight(const string_impl& from, const string_impl& to,
                           double weight) {
    add_weight(make_editpair(from, to), weight);
}

void WeightSet::add_weight(const EditPair& edit, double weight) {
    for (const auto& s : edit.first)
        _input_symbols.insert(s);
    _weights.insert(std::make_pair(edit, weight));
}

double WeightSet::get_weight(const string_impl& from,
                             const string_impl& to) const {
    return get_weight(make_editpair(from, to));
}

double WeightSet::get_weight(const EditPair& pair) const {
    if (_weights.count(pair) > 0)
        return _weights.at(pair);
    if (pair.first == pair.second)
        return default_identity_cost() * pair.first.size();
    if (pair.first.size() > 1 || pair.second.size() > 1)
        return calculate_wld(pair);
    if (pair.first.empty())
        return default_insertion_cost();
    if (pair.second.empty())
        return default_deletion_cost();
    return default_replacement_cost();
}

double WeightSet::calculate_wld(const EditPair& pair) const {
    return wld(pair, *this);
}

void WeightSet::divide_all(double divisor) {
    for (auto& elem : _weights)
        elem.second /= divisor;
}
}  // namespace WLD
}  // namespace Normalizer
}  // namespace Norma
