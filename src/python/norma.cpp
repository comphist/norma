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
#include<vector>
#include<boost/python.hpp>  //NOLINT[build/include_order]
#include<boost/python/suite/indexing/vector_indexing_suite.hpp>  //NOLINT[build/include_order]
#include"string_impl_conv.h"
#include"result_conv.h"
#include"training_conv.h"
#include"exception_wrapper.h"
#include"lexicon_wrapper.h"

namespace bp = boost::python;

namespace Norma {
namespace Python {

BOOST_PYTHON_MODULE(norma) {
    register_exception_translators();
    register_string_impl_converters();
    register_result_converters();
    register_training_converters();

    // Without the "true" template parameter, conversion of string_impl
    // will fail, and nobody knows why except for an obscure mailing
    // list post: <http://osdir.com/ml/python-c++/2004-03/msg00123.html>
    bp::class_<std::vector<string_impl>>("StringVector")
        .def(bp::vector_indexing_suite<std::vector<string_impl>, true>());

    result_wrapper::wrap();                // Result
    lexicon_wrapper::wrap();               // Lexicon
}

}  // namespace Python
}  // namespace Norma
