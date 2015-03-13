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
#include"implode_explode.h"
#include<vector>
#include"string_impl.h"

namespace Gfsm {

std::vector<string_impl> explode(const string_impl& str, bool att) {
    std::vector<string_impl> vec;
    bool braces = false;
    string_impl sym = "";
    for (string_size i = 0; i < str.length(); ++i) {
        if (braces) {
            if (str[i] == ']') {
                vec.push_back(sym);
                braces = false;
            } else {
                sym += from_char(str[i]);
            }
        } else if (att && str[i] == '[') {
            sym = "";
            braces = true;
        } else if (str[i] == ' ') {
            vec.push_back("<spc>");
        } else if (str[i] == '\t') {
            vec.push_back("<tab>");
        } else {
            vec.push_back(from_char(str[i]));
        }
    }
    return vec;
}

string_impl implode(const std::vector<string_impl>& vec, bool att) {
    string_impl str = "";
    for (const string_impl& c : vec) {
        if (att && c.length() > 1) {
            if (c == "<spc>")
                str += " ";
            else if (c == "<tab>")
                str += "\t";
            else
                str += "[" + c + "]";
        } else {
            str += c;
        }
    }
    return str;
}

}  // namespace Gfsm
