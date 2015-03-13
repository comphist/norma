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
#include"lexicon_wrapper.h"
#include<string>
#include<boost/python.hpp>  //NOLINT[build/include_order]
#include"normalizer/lexicon.h"

namespace Norma {
namespace Python {
using Norma::Normalizer::Lexicon;

void lexicon_wrapper::init(Lexicon* l,
                           std::string lexfile, std::string symfile) {
    l->set_lexfile(lexfile);
    l->set_symfile(symfile);
    l->init();
}

void lexicon_wrapper::save(Lexicon* l,
                           std::string lexfile, std::string symfile) {
    l->set_lexfile(lexfile);
    l->set_symfile(symfile);
    l->save_params();
}

void lexicon_wrapper::wrap() {
    namespace bp = boost::python;
    bp::class_<Lexicon>("Lexicon")
        // functions inherited from LexiconInterface
        // --note: I'm not modelling the inheritance for the time
        // being since we're opening several cans of worms here,
        // e.g. LexiconInterface having pure virtual functions
        .def("init", static_cast<void(Lexicon::*)()>(&Lexicon::init),
             "Initialize the lexicon.\n\n"
             "Loads data from any existing parameter files that have "
             "previously been set."
             )
        .def("init", &init,
             "Initialize the lexicon.\n\n"
             "Loads data from the supplied parameter files.\n\n"
             "Arguments:\n"
             "  lexfile -- Name of the lexicon file\n"
             "  symfile -- Name of the symbols file"
             )
        .def("add", &Lexicon::add,
             "Add a new lexicon entry.\n\n"
             "Arguments:\n"
             "  word -- Word to be added to the lexicon"
             )
        .def("clear", &Lexicon::clear,
             "Clear all lexicon entries."
             )
        .def("contains", &Lexicon::contains,
             "Check if the lexicon contains the given word.\n\n"
             "Arguments:\n"
             "  word -- Word to look up in the lexicon\n\n"
             "Returns:\n"
             "  True or False"
             )
        .def("contains_partial", &Lexicon::contains_partial,
             "Check if the lexicon contains at least one word starting with "
             "the given string.\n\n"
             "Arguments:\n"
             "  part -- The substring to look up\n\n"
             "Returns:\n"
             "  True or False"
             )
        .def("save", &Lexicon::save_params,
             "Save the lexicon to a file.\n\n"
             "Saves the lexicon to the parameter files in 'lexfile' and "
             "'symfile'."
             )
        .def("save", &save,
             "Save the lexicon to a file.\n\n"
             "Saves the lexicon to the given parameter files.\n\n"
             "Arguments:\n"
             "  lexfile -- Name of the lexicon file\n"
             "  symfile -- Name of the symbols file"
             )
        .add_property("entries", &Lexicon::entries,
                      "A StringSet object containing all lexicon entries.\n\n"
                      "Can mostly be used like a Python container, or "
                      "be explicitly converted to one, e.g., via "
                      "[x for x in lexicon.entries]."
                      )
        // magic methods
        .def("__contains__", &Lexicon::contains)
        .def("__len__", &Lexicon::size)
        // functions specific to Lexicon
        .add_property("lexfile", &Lexicon::get_lexfile, &Lexicon::set_lexfile,
                      "Name of the file containing the lexicon representation."
                      )
        .add_property("symfile", &Lexicon::get_symfile, &Lexicon::set_symfile,
                      "Name of the file containing symbols for lexfile."
                      )
        .add_static_property("symbol_boundary",
                             bp::make_getter(&Lexicon::SYMBOL_BOUNDARY,
                               bp::return_value_policy<bp::return_by_value>()))
        .add_static_property("symbol_any",
                             bp::make_getter(&Lexicon::SYMBOL_ANY,
                               bp::return_value_policy<bp::return_by_value>()))
        .add_static_property("symbol_epsilon",
                             bp::make_getter(&Lexicon::SYMBOL_EPSILON,
                               bp::return_value_policy<bp::return_by_value>()))
        .def("optimize", &Lexicon::optimize,
             "Optimize the internal representation of the Lexicon.\n\n"
             "This function should be called after constructing a lexicon "
             "from scratch with a large number of entries, as it will "
             "both reduce the size of saved lexicon files and increase "
             "performance during lookup.");
}

}  // namespace Python
}  // namespace Norma
