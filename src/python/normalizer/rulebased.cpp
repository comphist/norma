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
#include"normalizer/rulebased.h"
#include"python/normalizer/normalizer_wrapper.h"

namespace bp = boost::python;
namespace Norma {
namespace Python {
struct rulebased : normalizer_wrapper {
    static void wrap_rulebased() {
        using Norma::Normalizer::Rulebased::Rulebased;
        bp::docstring_options local_docstring_options(true, true, false);

        base_wrapper<Rulebased>::make_class("RulebasedNormalizer")
            .def("init", &init_nolex<Rulebased, &Rulebased::set_rulesfile>,
                 "Initialize the normalizer.\n\n"
                 "Initializes the normalizer with data from the supplied "
                 "parameter file.\n\n"
                 "Arguments:\n"
                 "  file -- Name of the parameter file"
                 )
            .def("init", &init<Rulebased, &Rulebased::set_rulesfile>,
                 bp::with_custodian_and_ward<1, 3>(),
                 "Initialize the normalizer.\n\n"
                 "Initializes the normalizer with data from the supplied "
                 "parameter file and a lexicon.\n\n"
                 "Arguments:\n"
                 "  file -- Name of the parameter file\n"
                 "   lex -- A Lexicon object"
                 )
            .def("save", &save<Rulebased, &Rulebased::set_rulesfile>,
                 "Save the normalizer state to a file.\n\n"
                 "Saves the current state of the normalizer to the given "
                 "parameter file.\n\n"
                 "Arguments:\n"
                 "  file -- Name of the file to save to"
                 )
            .def("clear_cache", &Rulebased::clear_cache,
                 "Clear the internal cache."
                 )
            .add_property("caching",
                          &Rulebased::is_caching, &Rulebased::set_caching,
                          "Whether to cache normalization results.\n\n"
                          "Caching increases performance at the cost of "
                          "higher memory usage.  The cache is only used when "
                          "a single best normalization candidate is requested, "
                          "never when determining the n-best candidates. "
                          "It is recommended to always keep this set to True."
                          )
            .add_property("rulesfile",
                          bp::make_function(&Rulebased::get_rulesfile,
                              bp::return_value_policy<bp::return_by_value>()),
                          bp::make_function(&Rulebased::set_rulesfile,
                                            bp::return_self<>()),
                          "Name of a parameter file containing the rules.")
            ;  // NOLINT[whitespace/semicolon]
    }
};

BOOST_PYTHON_MODULE(rulebased) {
    rulebased::wrap_rulebased();
}
}  // namespace Python
}  // namespace Norma


