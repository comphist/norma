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
#ifndef NORMALIZER_RESULT_H_
#define NORMALIZER_RESULT_H_
#include<limits>
#include<string>
#include<vector>
#include<tuple>
#include<queue>
#include"string_impl.h"

namespace Norma {
namespace Normalizer {
enum class LogLevel {
    TRACE = 0,
    WARN,
    ERROR,
    SILENT
};
typedef std::tuple<LogLevel, std::string, std::string> LogMessage;
LogMessage make_message(LogLevel loglevel,
                        std::string origin, std::string message);
std::string level_string(LogLevel loglevel);

struct Result {
    string_impl word = "";
    double score = 0.0;
    std::string origin = "<none>";
    unsigned int priority = std::numeric_limits<unsigned int>::max();
    std::queue<LogMessage> messages;

    Result() {}
    Result(const string_impl& w, double s) : word(w), score(s) {}
    Result(const string_impl& w, double s, const std::string& c)
    : word(w), score(s), origin(c) {}
    bool operator<(const Result& that) const {
        return this->score < that.score;
    }
    bool operator>(const Result& that) const {
        return this->score > that.score;
    }
    bool operator==(const Result& that) const {
        return this->score == that.score
            && this->word == that.word;
    }
    bool operator!=(const Result& that) const {
        return !(*this == that);
    }
};

typedef std::vector<Result> ResultSet;
}  // namespace Normalizer
}  // namespace Norma
#endif  // NORMALIZER_RESULT_H_

