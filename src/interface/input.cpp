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
#include<vector>
#include<string>
#include<iostream>
#include<stdexcept>
#include<algorithm>
#include<cctype>
#include<functional>
#include"string_impl.h"
#include"cycle.h"

using std::string;
using std::function;

namespace Norma {
///////////////////////////// Input //////////////////////////////////////
string_impl Input::get_line() {
    std::string l;
    getline(*_input, l);
    _line = l.c_str();
    lower_case(&_line);
    return _line;
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
    _request_train = (string_find(line, " ") != string_npos);
    return line;
}

//////////////////////////// ShellInput //////////////////////////////////

ShellInput::ShellInput() {
    _input = &std::cin;
    _output = &std::cout;
    parse_command = CommandHandler(this, _cycle, _output);
}

string_impl ShellInput::get_line() {
    *_output  << prompt;
    string_impl line = Input::get_line();
    std::string sline = to_cstr(line);
    if (line[0] == '!') {
        parse_command(sline);
        return "";
    }
    _line = line;
    _request_train = (string_find(line, " ") != string_npos);
    return line;
}

void CommandHandler::operator()(const string& command) {
    std::vector<string> tokens;
    size_t pos = 0,
           left = 1;  // 1 to ignore leading !
    while (pos != std::string::npos) {
        pos = command.find(" ", left);
        std::string token = command.substr(left, pos - left);
        if (token.length() != 0) {
            std::transform(token.begin(), token.end(),
                           token.begin(), ::tolower);
            tokens.push_back(token);
        }
        left = pos + 1;
    }
    if (tokens.size() == 1)
        tokens.push_back("");
    try {
        _commands[tokens[0]](this, tokens[1]);
    } catch(std::bad_function_call e) {
        *_output  << "*** Unknown syntax: " << command << std::endl;
    }
}

void CommandHandler::command_exit(const std::string& arg) {
    _si->do_quit = true;
}

void CommandHandler::command_help(const std::string& arg) {
    if (arg.length() == 0) {
        *_output  << "Documented commands (type help <topic>):"
                  << std::endl
                  << "========================================"
                  << std::endl;
        for (auto helpentry : _helptxt)
            *_output  << helpentry.first << " ";
        *_output  << std::endl << std::endl
                  << "Undocumented commands:" << std::endl
                  << "========================================"
                  << std::endl;
        for (auto command : _commands)
            if (_helptxt.count(command.first) == 0)
                *_output  << command.first << " ";
        *_output  << std::endl << std::endl;;
    } else {
        if (_helptxt.count(arg) != 0)
            *_output  << _helptxt[arg] << std::endl;
        else
            *_output  << "*** No help for " << arg << std::endl;
    }
}
void CommandHandler::command_save(const std::string& arg) {
    _cycle->save_params();
}
void CommandHandler::switch_feature(const string& desc, const string& arg,
                                function<void(Cycle*, bool)> set,
                                function<bool(Cycle*)> check) {  // NOLINT[readability/casting]
    if (arg == "on")
        set(_cycle, true);
    else if (arg == "off")
        set(_cycle, false);
    *_output  << "*** " << desc << " is "
              << (check(_cycle) ? "ON" : "OFF")
              << "." << std::endl;
}
void CommandHandler::command_prob(const string& arg) {
    switch_feature("Printing of probabilities", arg,
                   &Cycle::do_print_prob, &Cycle::print_prob);
}
void CommandHandler::command_train(const string& arg) {
    switch_feature("Training", arg,
                   &Cycle::do_train, &Cycle::train);
}
void CommandHandler::command_normalize(const string& arg) {
    switch_feature("Normalizing", arg,
                   &Cycle::do_norm, &Cycle::norm);
}
}  // namespace Norma

