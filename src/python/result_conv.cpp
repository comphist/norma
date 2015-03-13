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
// ----------------------------------------------------------------------------
// This will register converters with Boost::Python that convert
// Result and ResultSet into native Python tuples or lists of tuples.
// ----------------------------------------------------------------------------
#include"result_conv.h"
#include<string>
#include<boost/python.hpp>  //NOLINT[build/include_order]
#include"normalizer/result.h"

namespace bp = boost::python;
using Norma::Normalizer::Result;
using Norma::Normalizer::ResultSet;

namespace Norma {
namespace Python {
PyObject* Result_to_python_tuple::convert(Result const& r) {
    return bp::incref(bp::make_tuple(r.word, r.score, r.origin).ptr());
}

PyObject* ResultSet_to_python_list::convert(ResultSet const& rs) {
    bp::list l;
    for (Result const& r : rs) {
        l.append(r);
    }
    return bp::incref(l.ptr());
}

void register_result_converters() {
    bp::to_python_converter<Result, Result_to_python_tuple>();
    bp::to_python_converter<ResultSet, ResultSet_to_python_list>();
}
}  // namespace Python
}  // namespace Norma
