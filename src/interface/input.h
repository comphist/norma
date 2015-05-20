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
#ifndef INTERFACE_INPUT_H_
#define INTERFACE_INPUT_H_
#include<string>
#include<fstream>
#include<iosfwd>
#include<map>
#include<functional>
#include"string_impl.h"
#include"iobase.h"

namespace Norma {
class Cycle;

/// Basic Input class, pure virtual
/** Admittedly it doesn't make that much sense to have it as pure
 *  virtual, but FileInput needs the filename as parameter, and
 *  that shouldn't be in the common base class for Input, while
 *  ShellInput needs a whole lot of other stuff.
 *
 *  So essentially this doesn't serve any function other than
 *  providing a common base for polymorphic access, so we can't
 *  have our client code walking around with Input objects.
 *
 *  Hence, request_quit is 0 even though it could easily return
 *  _input->eof().
 **/
class Input : public IOBase {
 public:
     Input() = default;
     virtual ~Input() {}
     /// anything that should be done before the processing
     /// begins, such as greeting the user
     virtual void begin() {}
     /// anything happening after the processing itself starts
     virtual void end() {}
     /// the actual function that reads a line and passes it
     /// to Cycle
     virtual string_impl get_line();
     /// method to check if the Input requested program termination
     virtual bool request_quit() = 0;
 protected:
     virtual void store_line(const string_impl& line) {
         _training->add_source(line);
     }
     std::istream *_input;
};

/// Input from a file
class FileInput : public Input {
 public:
     explicit FileInput(const std::string& fname);
     ~FileInput();
     string_impl get_line();
     inline bool request_quit() {
         return _input->eof();
     }
     bool thread_suitable() {
         return true;
     }
 private:
     std::ifstream _file;
     std::ostream *_output, *_error;
};

class ShellInput;
class CommandHandler {
    friend class ShellInput;
 protected:
    CommandHandler() = default;
    CommandHandler(ShellInput* si, Cycle* cycle, std::ostream* output) {
        _si = si;
        _cycle = cycle;
        _output = output;
    }
    void operator()(const std::string& command);

 private:
    ShellInput* _si;
    Cycle* _cycle;
    std::ostream* _output;
    ///////////////////////////////////////////////////
    // from here on, member functions that implement
    // commands. parameter is the argument to the command.
    ///////////////////////////////////////////////////
    void command_exit(const std::string& arg);
    void command_help(const std::string& arg);
    void command_prob(const std::string& arg);
    void command_save(const std::string& arg);
    void command_train(const std::string& arg);
    void command_normalize(const std::string& arg);
    ///////////////////////////////////////////////////
    /// a generic function to toggle various features
    /// the Cycle may offer.
    void switch_feature(const std::string& desc,
                        const std::string& arg,
                        std::function<void(Cycle*, bool)> set,
                        std::function<bool(Cycle*)> check);
    std::map<std::string, std::string> _helptxt
         = { { "exit", "Exits the program, saving data if necessary." },
             { "prob", "Turn the printing of probabilities on or off, "
                       "or show the current setting." },
             { "save", "Save the parameter files of all active "
                       "normalizers." },
             { "train", "Turn the 'training' option on or off, or show "
                        "the current setting." },
             { "normalize", "Turn the 'normalizing' option on or off, or"
                            "show the current setting." } };
    std::map<std::string,
         std::function<void(CommandHandler*, const std::string&)>> _commands
     = { { "exit", &CommandHandler::command_exit },
         { "help", &CommandHandler::command_help },
         { "prob", &CommandHandler::command_prob },
         { "save", &CommandHandler::command_save },
         { "normalize", &CommandHandler::command_normalize },
         { "train", &CommandHandler::command_train } };
};
/// Input from a shell
/** Adding a command to the shell:
 *  1. Implement it as class method. The method receives
 *     one argument for a command as parameter. The argument
 *     is downcased.
 *  2. add an entry to commands with the name of the command
 *     and the pointer to the unbound method
 *  3. (Optional): Add a help text to _helptxt
 **/
class ShellInput : public Input {
    friend class CommandHandler;
 public:
     ShellInput();
     inline void begin() {
         *_output << intro << std::endl;
     }
     inline void end() {
         *_output << std::endl;
     }
     string_impl get_line();
     bool request_quit() {
         return _input->eof() || do_quit;
     }

 protected:
     bool do_quit = false;

 private:
     std::ostream *_output;
     CommandHandler parse_command;
     const char *prompt = "> ",
                *intro  = "\nEnter any wordform to normalize it."
                          "\nPrefix a word with '!' to issue commands. "
                          "For a list of all commands, type '!help'.\n"
                          "Exit with CTRL+D or '!exit'.";
};
}  // namespace Norma
#endif  // INTERFACE_INPUT_H_

