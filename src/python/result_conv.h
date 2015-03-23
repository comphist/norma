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
#ifndef NORMA_PYTHON_RESULT_CONV_H_
#define NORMA_PYTHON_RESULT_CONV_H_
#include<string>
#include<boost/python.hpp>  //NOLINT[build/include_order]
#include"string_impl.h"
#include"normalizer/result.h"

namespace Norma {
namespace Python {
typedef boost::python::converter::rvalue_from_python_stage1_data
    py_stage1_data;
typedef boost::python::converter::rvalue_from_python_storage<string_impl>
    py_storage;

struct Result_from_python_tuple {
    static void* convertible(PyObject* obj_ptr);
    static void  construct(PyObject* obj_ptr,
                           py_stage1_data* data);
};

struct ResultSet_to_python_list {
    static PyObject* convert(Norma::Normalizer::ResultSet const& rs);
};

void register_result_converters();

struct result_wrapper {
    static string_impl repr(const Norma::Normalizer::Result& result);
    static void wrap();
};
}  // namespace Python
}  // namespace Norma

#endif  // NORMA_PYTHON_RESULT_CONV_H_
