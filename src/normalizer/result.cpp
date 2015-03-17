/* Copyright 2015 Marcel Bollmann, Florian Petran
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
#include<tuple>
#include<string>
#include"result.h"

namespace Norma {
namespace Normalizer {
LogMessage make_message(LogLevel loglevel,
                        std::string origin, std::string message) {
    return std::make_tuple(loglevel, origin, message);
}

std::string level_string(LogLevel loglevel) {
    switch(loglevel) {
        case LogLevel::TRACE:
            return "TRACE";
        case LogLevel::WARN:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
    }
}
}  // namespace Normalizer
}  // namespace Norma
