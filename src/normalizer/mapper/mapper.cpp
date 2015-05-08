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
#include"mapper.h"
#include<algorithm>
#include<functional>
#include<map>
#include<sstream>
#include<stdexcept>
#include<string>
#include<tuple>
#include<vector>
#include"normalizer/exceptions.h"
#include"normalizer/result.h"

using std::map;
using std::string;

namespace Norma {
namespace Normalizer {
namespace Mapper {

void Mapper::set_from_params(const std::map<std::string, std::string>& params) {
    if (params.count("Mapper.mapfile") != 0)
        set_mapfile(to_absolute(params.at("Mapper.mapfile"), params));
    else if (params.count("perfilemode.input") != 0)
        set_mapfile(with_extension(params.at("perfilemode.input"),
                                   "Mapper.mapfile"));
}

void Mapper::init() {
    clear();
    if (!_mapfile.empty())
        read_mapfile(_mapfile);
}

void Mapper::clear() {
    _map.clear();
}

Result Mapper::operator()(const string_impl& word) const {
    // this will probably perform worse than the implementation it had before
    // since an entire list is sorted instead of just returning the best result,
    // but it's a cleaner implementation. if the performance hit is significant,
    // it should be changed to how it was before.
    ResultSet resultset = make_all_results(word);
    if (resultset.size() == 0) {
        Result not_found = make_result(word, 0.0);
        return not_found;
    }
    return resultset.front();
}

ResultSet Mapper::operator()(const string_impl& word, unsigned int n) const {
    ResultSet resultset = make_all_results(word);
    if (resultset.size() > n)
        resultset.resize(n);
    return resultset;
}

ResultSet Mapper::make_all_results(const string_impl& word) const {
    if (_map.count(word) == 0)
        return ResultSet();
    ResultSet resultset;
    const std::map<string_impl, int>& row = _map.at(word);
    double total_count = 0.0;

    for (auto& entry : row) {
        Result result = make_result(entry.first, entry.second);
        resultset.push_back(result);
        total_count += entry.second;
    }

    for (auto& result : resultset)
        result.score /= total_count;
    std::sort(resultset.begin(), resultset.end(), std::greater<Result>());
    return resultset;
}

bool Mapper::train(TrainingData* data) {
    for (auto pp = data->rbegin(); pp != data->rend(); ++pp) {
        if (pp->is_used())
            break;
        this->train(pp->source(), pp->target(), 1);
    }
    return true;
}

void Mapper::train(const string_impl& word,
                   const string_impl& modern,
                   int count) {
    if (_map.count(word) == 0 || _map[word].count(modern) == 0)
        _map[word][modern] = count;
    else
        _map[word][modern] += count;
}

void Mapper::save_params() {
    write_mapfile(_mapfile);
}

bool Mapper::read_mapfile(const std::string& fname) {
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
        string_impl word,
                    modern;
        int count;
        iss >> word >> modern >> count;
        if (iss)
            train(word, modern, count);
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

bool Mapper::write_mapfile(const std::string& fname) {
    std::ofstream file;
    file.open(fname, std::ios::trunc);
    if (!file.is_open())
        return false;
    for (auto row : _map)
        for (auto entry : row.second)
            file << row.first << "\t"
                 << entry.first << "\t"
                 << entry.second << std::endl;
    file.close();
    return true;
}
}  // namespace Mapper
}  // namespace Normalizer
}  // namespace Norma

