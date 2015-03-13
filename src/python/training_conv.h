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
#ifndef NORMA_PYTHON_TRAINING_CONV_H_
#define NORMA_PYTHON_TRAINING_CONV_H_
#include<string>
#include<boost/python.hpp>  //NOLINT[build/include_order]
#include"training_data.h"

namespace Norma {
namespace Python {
typedef boost::python::converter::rvalue_from_python_stage1_data
    py_stage1_data;
typedef boost::python::converter::rvalue_from_python_storage<string_impl>
    py_storage;

struct TrainingPair_to_python_tuple {
    static PyObject* convert(Norma::TrainingPair const& pair);
};

struct TrainingData_from_python_list {
    static void* convertible(PyObject* obj_ptr);
    static void  construct(PyObject* obj_ptr,
                           py_stage1_data* data);
};

// register string_impl converters before calling this!
void register_training_converters();
}  // namespace Python
}  // namespace Norma

#endif  // NORMA_PYTHON_TRAINING_CONV_H_
