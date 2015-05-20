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
#include"rulebased.h"
#include<map>
#include<mutex>
#include<string>
#include"normalizer/result.h"
#include"normalizer/cacheable.h"
#include"interface/iobase.h"
#include"rule.h"
#include"candidate_finder.h"

namespace Norma {
namespace Normalizer {
namespace Rulebased {

void Rulebased::set_from_params(const std::map<std::string, std::string>&
                                                                       params) {
    if (params.count(_name + ".rulesfile") != 0)
        set_rulesfile(to_absolute(params.at(_name + ".rulesfile"), params));
    else if (params.count("perfilemode.input") != 0)
        set_rulesfile(with_extension(params.at("perfilemode.input"),
                                     _name + ".rulesfile"));
}

void Rulebased::init() {
    clear();
    if (!_rulesfile.empty())
        _rules.read_rulesfile(_rulesfile);
}

void Rulebased::clear() {
    _rules.clear();
    clear_cache();
}

Result Rulebased::do_normalize(const string_impl& word) const {
    if (is_caching()) {
        Result res = query_cache(word);
        if (res != Result())
            return res;
    }

    ResultSet resultset = do_normalize(word, 1);
    Result result;
    if (resultset.empty()) {
        result = Result(word, 0.0, name());
        log_message(&result, LogLevel::TRACE, "no candidate found");
    } else {
        result = resultset.front();
    }
    if (is_caching())
        cache(word, result);
    return result;
}

ResultSet Rulebased::do_normalize(const string_impl& word,
                                  unsigned int n) const {
    ResultSet resultset;
    Result unchanged_result = make_result(word, 0.0);
    CandidateFinder finder(word, _rules, *_lex, _name);
    for (unsigned int i = 0; i < n; ++i) {
        Result result = finder();
        if (result == unchanged_result)
            break;
        resultset.push_back(result);
    }
    return resultset;
}

bool Rulebased::do_train(TrainingData* data) {
    for (auto pp = data->rbegin(); pp != data->rend(); ++pp) {
        if (pp->is_used())
            break;
        _rules.learn_ruleset(learn_rules(pp->source(),
                                         pp->target(),
                                         true, true));
    }
    clear_cache();
    return true;
}

void Rulebased::do_save_params() {
    _rules.save_rulesfile(_rulesfile);
}

}  // namespace Rulebased
}  // namespace Normalizer
}  // namespace Norma
