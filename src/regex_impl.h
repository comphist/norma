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
#ifndef REGEX_IMPL_H_
#define REGEX_IMPL_H_
#include<string>
#include<boost/regex.hpp>  // NOLINT[build/include_order]
#include"defines.h"  // NOLINT[build/include_order]

#ifdef USE_ICU_STRING
#include<boost/regex/icu.hpp>  // NOLINT[build/include_order]

typedef boost::u32regex regex_impl;
#define REGEX_IMPL_MATCH boost::u32regex_match

inline regex_impl make_regex_impl(const std::string& regex_str) {
    return boost::make_u32regex(regex_str.c_str());
}

#else  // USE_ICU_STRING

typedef boost::regex regex_impl;
#define REGEX_IMPL_MATCH boost::regex_match

inline regex_impl make_regex_impl(const std::string& regex_str) {
    return boost::regex(regex_str);
}

#endif  // USE_ICU_STRING

#endif  // REGEX_IMPL_H
