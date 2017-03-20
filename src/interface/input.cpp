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
#include"input.h"
#include<string>
#include<iostream>
#include<stdexcept>
#include"string_impl.h"
#include"cycle.h"

using std::string;

namespace Norma {
///////////////////////////// Input //////////////////////////////////////
string_impl Input::get_line() {
    std::string l;
    getline(*_input, l);
    return l;
}

//////////////////////////// FileInput ///////////////////////////////////
FileInput::FileInput(const std::string& fname)
    : Input() {
    _file.open(fname);
    if (!_file.is_open())
        throw std::runtime_error("Could not open input file!");
    _input  = &_file;
    _output = &std::cout;
    _error  = &std::cerr;
}

FileInput::~FileInput() {
    try {
        _file.close();
    } catch(...) {
        *_error << "Error while closing input file!" << std::endl;
    }
}

string_impl FileInput::get_line() {
    string_impl line = Input::get_line();
    _request_train = (string_find(line, "\t") != string_npos);
    return line;
}

}  // namespace Norma

