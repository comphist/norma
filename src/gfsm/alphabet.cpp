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
#include"alphabet.h"
#include<string>
#include<fstream>
#include<sstream>
#include<stdexcept>
#include<set>
#include<vector>
#include<boost/bimap.hpp>  // NOLINT[build/include_order]
#include"string_impl.h"
#include"labelvector.h"

namespace Gfsm {

void Alphabet::clear() {
    _alph.clear();
    _next_free_label = 1;
    _map_unknown = nullptr;
}

void Alphabet::load_labfile(const std::string& filename) {
    clear();
    std::ifstream file;
    file.open(filename);
    if (!file.is_open())
        throw std::runtime_error("Could not open labels file.");
    std::string line = "";
    while (getline(file, line)) {
        if (file.eof())
            break;
        std::istringstream iss(line);
        string_impl symbol;
        gfsmLabelVal label;
        iss >> symbol >> label;
        if (iss.fail() || iss.bad())
            throw std::runtime_error("Error parsing labels file!");
        if (label > _next_free_label)
            _next_free_label = label;
        _alph.insert(AlphabetMapping(symbol, label));
    }
    file.close();
    _next_free_label++;
}

void Alphabet::save_labfile(const std::string& filename) const {
    std::ofstream file;
    file.open(filename);
    if (!file.is_open())
        throw std::runtime_error("Could not open labels file for saving.");
    auto left = _alph.left;
    for (auto pair = left.begin(), pairend = left.end();
         pair != pairend; ++pair) {
        file << pair->first << "\t" << pair->second << std::endl;
    }
    file.close();
}

void Alphabet::add_mapping(const string_impl& symbol,
                           const gfsmLabelVal& label) {
    _alph.insert(Alphabet::AlphabetMapping(symbol, label));
    if (label >= _next_free_label)
        _next_free_label = label + 1;
}

gfsmLabelVal Alphabet::add_symbol(const string_impl& symbol) {
    if (contains(symbol))
        return get_label(symbol);
    gfsmLabelVal val = _next_free_label;
    add_mapping(symbol, val);
    return val;
}

bool Alphabet::contains(const string_impl& symbol) const {
    return (_alph.left.count(symbol) > 0);
}

bool Alphabet::contains(const gfsmLabelVal& label) const {
    return (_alph.right.count(label) > 0);
}

gfsmLabelVal Alphabet::get_label(const string_impl& symbol) const {
    return _alph.left.at(symbol);
}

string_impl Alphabet::get_symbol(const gfsmLabelVal& label) const {
    return _alph.right.at(label);
}

gfsmLabelVal Alphabet::map_symbol(const string_impl& symbol) const {
    if (contains(symbol)) {
        return get_label(symbol);
    }
    if (_map_unknown != nullptr) {
        const string_impl new_symbol = (*_map_unknown)(symbol);
        if (contains(new_symbol)) {
            return get_label(new_symbol);
        }
    }
    return 0;
}

LabelVector Alphabet::map_symbols(const string_impl& symbols) const {
    string_impl c;
    LabelVector vec;
    for (string_size i = 0; i < symbols.length(); ++i) {
        c = from_char(symbols[i]);
        gfsmLabelVal val = map_symbol(c);
        if (val > 0 || !_ignore_unknowns) {
            vec.push_back(val);
        }
    }
    return vec;
}

LabelVector
        Alphabet::map_symbols(const std::vector<string_impl>& symbols) const {
    LabelVector vec;
    for (auto c : symbols) {
        gfsmLabelVal val = map_symbol(c);
        if (val > 0 || !_ignore_unknowns) {
            vec.push_back(val);
        }
    }
    return vec;
}

string_impl Alphabet::map_labels(const LabelVector& labels) const {
    string_impl s = "";
    for (auto lab : labels) {
        if (contains(lab)) {
            s += get_symbol(lab);
        }
    }
    return s;
}

std::vector<string_impl>
        Alphabet::map_labels_to_vector(const LabelVector& labels) const {
    std::vector<string_impl> v;
    for (auto lab : labels) {
        if (contains(lab)) {
            v.push_back(get_symbol(lab));
        }
    }
    return v;
}

LabelVector Alphabet::cover(const string_impl& symbols) {
    string_impl c;
    LabelVector vec;
    for (string_size i = 0; i < symbols.length(); ++i) {
        gfsmLabelVal val;
        c = from_char(symbols[i]);
        if (contains(c)) {
            val = get_label(c);
        } else {
            val = add_symbol(c);
        }
        vec.push_back(val);
    }
    return vec;
}

LabelVector Alphabet::cover(const std::vector<string_impl>& symbols) {
    LabelVector vec;
    for (auto c : symbols) {
        gfsmLabelVal val;
        if (contains(c)) {
            val = get_label(c);
        } else {
            val = add_symbol(c);
        }
        vec.push_back(val);
    }
    return vec;
}

LabelVector Alphabet::cover(const std::set<string_impl>& symbols) {
    LabelVector vec;
    for (auto c : symbols) {
        gfsmLabelVal val;
        if (contains(c)) {
            val = get_label(c);
        } else {
            val = add_symbol(c);
        }
        vec.push_back(val);
    }
    return vec;
}

std::set<string_impl> Alphabet::covered() const {
    std::set<string_impl> result;
    for (const auto& mapping : _alph.left) {
        result.insert(mapping.first);
    }
    return result;
}

}  // namespace Gfsm
