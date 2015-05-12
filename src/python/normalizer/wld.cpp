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
#include"python/normalizer/normalizer_wrapper.h"
#include"normalizer/wld.h"

namespace bp = boost::python;
namespace Norma {
namespace Python {
struct wld : normalizer_wrapper {
    static void wrap_wld() {
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
};

BOOST_PYTHON_MODULE(wld) {
    wld::wrap_wld();
}
}  // namespace Python
}  // namespace Norma

