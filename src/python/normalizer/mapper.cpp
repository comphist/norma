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
#include"normalizer/mapper.h"
#include"python/normalizer/normalizer_wrapper.h"

namespace bp = boost::python;

namespace Norma {
namespace Python {
struct mapper : normalizer_wrapper {
    static void wrap_mapper() {
        using Norma::Normalizer::Mapper::Mapper;
        void (Mapper::*mapper_train)(const string_impl&,
                                     const string_impl&, int) = &Mapper::train;
        bp::docstring_options local_docstring_options(true, true, false);

        base_wrapper<Mapper>::make_class("MapperNormalizer")
            .def("init", &init_nolex<Mapper, &Mapper::set_mapfile>,
                 "Initialize the normalizer.\n\n"
                 "Initializes the normalizer with data from the supplied "
                 "parameter file.\n\n"
                 "Arguments:\n"
                 "  file -- Name of the parameter file"
                 )
            .def("init", &init<Mapper, &Mapper::set_mapfile>,
                 bp::with_custodian_and_ward<1, 3>(),
                 "Initialize the normalizer.\n\n"
                 "This overload is not useful for this normalizer -- it only "
                 "exists for compatibility reasons."
                 )
            .def("save", &save<Mapper, &Mapper::set_mapfile>,
                 "Save the normalizer state to a file.\n\n"
                 "Saves the current state of the normalizer to the given "
                 "parameter file.\n\n"
                 "Arguments:\n"
                 "  file -- Name of the file to save to"
                 )
            .def("train", mapper_train,
                 "Train the normalizer on a single word pair.\n\n"
                 "Arguments:\n"
                 "  source -- Input wordform\n"
                 "  target -- Normalized (target) wordform\n"
                 "       n -- Number of times this word pair has been seen\n\n"
                 "Calling train(source, target, n) is functionally identical "
                 "to calling train(n * [(source, target)])."
                 )
            .add_property("mapfile",
                          bp::make_function(&Mapper::get_mapfile,
                              bp::return_value_policy<bp::return_by_value>()),
                          bp::make_function(&Mapper::set_mapfile,
                                            bp::return_self<>()),
                          "Name of a parameter file containing the mappings.");
    }
};

BOOST_PYTHON_MODULE(mapper) {
    mapper::wrap_mapper();
}
}  // namespace Python
}  // namespace Norma

