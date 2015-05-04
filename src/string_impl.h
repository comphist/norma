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
//
// All string implementation stuff goes here. Provides
// wrapper for all operations and queries related to strings
// so that the actual code has the option of using std::string
// as well as UCI strings.
#ifndef STRING_IMPL_H_
#define STRING_IMPL_H_
#include"defines.h"  // NOLINT[build/include_order]

#ifdef USE_ICU_STRING
#include<istream>
#include<ostream>
#include<unicode/unistr.h> // NOLINT[build/include_order]
#include<unicode/uchar.h>  // NOLINT[build/include_order]

/// Wrapper class for UnicodeString.
/// this class is necessary because we assume that non-ICU
/// strings are always UTF encoded already, which is not an
/// assumption ICU shares, so we have to explicitly call
/// UnicodeString::fromUTF8 a few times.
class unicode_string_impl : public UnicodeString {
 public:
     unicode_string_impl() = default;
     unicode_string_impl(const char* that)
         : UnicodeString(UnicodeString::fromUTF8(that)) {}
     unicode_string_impl(char that) {
         char tmp[2];
         tmp[0] = that; tmp[1] = '\0';
         UnicodeString(UnicodeString::fromUTF8(tmp));
     }
     unicode_string_impl(const std::string& that)
         : UnicodeString(UnicodeString::fromUTF8(that)) {}
     unicode_string_impl(const UChar32& that)
        : UnicodeString(that) {}
     unicode_string_impl(const UnicodeString& that)
        : UnicodeString(that) {}
};

typedef unicode_string_impl string_impl;
typedef UChar32 char_impl;
typedef int string_size;

inline void lower_case(string_impl* str)
    { str->toLower(); }
inline void upper_case(string_impl* str)
    { str->toUpper(); }
static const string_size string_npos = -1;
inline void extract(const string_impl& str, int from, int to, string_impl* out)
    { str.extractBetween(from, to, *out); }
inline string_size string_find(const string_impl& me, const char* you)
    { return me.indexOf(you); }

inline bool check_if_alpha(char_impl c) {
    return u_isalpha(c);
}

const char* to_cstr(const string_impl&);

inline string_impl from_char(char_impl c) {
    return c;
}

inline bool is_empty(const string_impl& str) {
    return str.isEmpty();
}

std::istream& operator>>(std::istream& strm, string_impl& val);
std::ostream& operator<<(std::ostream& strm, const string_impl& ustr);

#else  // USE_ICU_STRING
#include<algorithm>
#include<string>
#include<sstream>

typedef std::string string_impl;
typedef char char_impl;
typedef size_t string_size;

inline void lower_case(string_impl* str)
    { std::transform(str->begin(), str->end(), str->begin(), ::tolower); }

inline void upper_case(string_impl* str)
    { std::transform(str->begin(), str->end(), str->begin(), ::toupper); }

static const size_t string_npos = std::string::npos;

inline void extract(const string_impl& str, int from, int to, string_impl* out)
    { *out = str.substr(from, to); }

inline string_size string_find(const string_impl& me, const char* you)
    { return me.find(you); }

inline bool check_if_alpha(char_impl c) {
    return isalpha(c);
}

inline const char* to_cstr(const string_impl& str) {
    return str.c_str();
}

inline string_impl from_char(char_impl c) {
    std::stringstream ss;
    std::string s;
    ss << c;
    ss >> s;
    return s;
}

inline bool is_empty(const string_impl& str) {
    return str.empty();
}

#endif  // USE_ICU_STRING

void extract_tail(const string_impl& str, string_size len, string_impl* out);

bool has_alpha(const string_impl& str);

inline void upper_case(char_impl* c) {
    string_impl s = "";
    s += *c;
    upper_case(&s);
    *c = s[0];
}

#endif  // STRING_IMPL_H_

