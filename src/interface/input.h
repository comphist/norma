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

}  // namespace Norma
#endif  // INTERFACE_INPUT_H_

