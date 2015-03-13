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
#include"normalizer_wrapper.h"
#include<boost/python.hpp>  //NOLINT[build/include_order]
#include"normalizer/base.h"
#include"normalizer/mapper.h"
#include"normalizer/rulebased.h"
#include"normalizer/wld.h"

namespace bp = boost::python;
using Norma::Normalizer::Base;

namespace Norma {
namespace Python {

void normalizer_wrapper::wrap_Mapper() {
    using Norma::Normalizer::Mapper;
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
             "Calling train(source, target, n) is functionally identical to "
             "calling train(n * [(source, target)])."
             )
        .add_property("mapfile",
                      bp::make_function(&Mapper::get_mapfile,
                          bp::return_value_policy<bp::return_by_value>()),
                      bp::make_function(&Mapper::set_mapfile,
                                        bp::return_self<>()),
                      "Name of a parameter file containing the mappings.");
}

void normalizer_wrapper::wrap_Rulebased() {
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
                      "Name of a parameter file containing the rules.");
}

void normalizer_wrapper::wrap_WLD() {
    using Norma::Normalizer::WLD::WLD;
    bp::docstring_options local_docstring_options(true, true, false);

    base_wrapper<WLD>::make_class("WLDNormalizer")
        .def("init", &init_nolex<WLD, &WLD::set_paramfile>,
             "Initialize the normalizer.\n\n"
             "Initializes the normalizer with data from the supplied "
             "parameter file.\n\n"
             "Arguments:\n"
             "  file -- Name of the parameter file"
             )
        .def("init", &init<WLD, &WLD::set_paramfile>,
             bp::with_custodian_and_ward<1, 3>(),
             "Initialize the normalizer.\n\n"
             "Initializes the normalizer with data from the supplied "
             "parameter file and a lexicon.\n\n"
             "Arguments:\n"
             "  file -- Name of the parameter file\n"
             "   lex -- A Lexicon object"
             )
        .def("save", &save<WLD, &WLD::set_paramfile>,
             "Save the normalizer state to a file.\n\n"
             "Saves the current state of the normalizer to the given "
             "parameter file.\n\n"
             "Arguments:\n"
             "  file -- Name of the file to save to"
             )
        .def("perform_training", &WLD::perform_training,
             "Perform the training algorithm.\n\n"
             "The WLD normalizer has the restriction that it cannot be "
             "trained incrementally.  Therefore, calling train() only "
             "stores the training data internally; call this function to "
             "perform the actual training.\n\n"
             "(This behaviour is an ugly hack and may change in the "
             "future.)"
             )
        .def("clear_cache", &WLD::clear_cache,
             "Clear the internal cache."
             )
        .add_property("caching",
                      &WLD::is_caching, &WLD::set_caching,
                      "Whether to cache normalization results.\n\n"
                      "Caching increases performance at the cost of "
                      "higher memory usage.  The cache is only used when "
                      "a single best normalization candidate is requested, "
                      "never when determining the n-best candidates. "
                      "It is recommended to always keep this set to True."
                      )
        .add_property("ngrams",
                      &WLD::get_train_ngrams,
                      bp::make_function(&WLD::set_train_ngrams,
                                        bp::return_self<>()),
                      "The number of character n-grams to consider during "
                      "training."
                      )
        .add_property("divisor",
                      &WLD::get_train_divisor,
                      bp::make_function(&WLD::set_train_divisor,
                                        bp::return_self<>()),
                      "A divisor to apply to all weights generated during "
                      "training."
                      )
        .add_property("max_weight",
                      &WLD::get_maximum_weight,
                      bp::make_function(&WLD::set_maximum_weight,
                                        bp::return_self<>()),
                      "Maximum allowed weight of normalization candidates "
                      "(0 = no maximum)."
                      )
        .add_property("max_ops",
                      &WLD::get_maximum_ops,
                      bp::make_function(&WLD::set_maximum_ops,
                                        bp::return_self<>()),
                      "Maximum number of finite-state operations during "
                      "normalization (0 = no maximum)."
                      )
        .add_property("paramfile",
                      bp::make_function(&WLD::get_paramfile,
                          bp::return_value_policy<bp::return_by_value>()),
                      bp::make_function(&WLD::set_paramfile,
                                        bp::return_self<>()),
                      "Name of a parameter file containing the weights.");
}
}  // namespace Python
}  // namespace Norma
