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
#include"string_impl.h"

#ifdef USE_ICU_STRING
#include<string>
#include<istream>
#include<ostream>

const char* to_cstr(const string_impl& str) {
    // one problem with this function:
    // it returns a char ptr, so if i call it
    // twice, both char ptr will have the value
    // of the second call. might be fixable by
    // making the ptr non static, but idk what
    // that will do to performance.
    thread_local char out[256];
    out[str.extract(0, 99, out)] = 0;
    return out;
}

std::istream& operator>>(std::istream& strm, string_impl& val) {
    std::string str;
    strm >> str;
    string_impl conv(str.c_str());
    val = conv;
    return strm;
}

std::ostream& operator<<(std::ostream& strm, const string_impl& ustr) {
    std::string str = to_cstr(ustr);
    strm << str;
    return strm;
}

#endif  // USE_ICU_STRING

void extract_tail(const string_impl& str, string_size len, string_impl* out) {
    if (len >= str.length()) {
        *out = str;
    } else {
        extract(str, str.length()-len, str.length(), out);
    }
}

bool has_alpha(const string_impl& str) {
    for (string_size i = 0; i < str.length(); ++i)
        if (check_if_alpha(str[i]))
            return true;
    return false;
}

