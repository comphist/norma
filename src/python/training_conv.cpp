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
// TrainingPair and TrainingData to and from native Python objects.
// ----------------------------------------------------------------------------
#include"training_conv.h"
#include<boost/python.hpp>  //NOLINT[build/include_order]
#include"string_impl.h"
#include"training_data.h"

namespace bp = boost::python;

namespace Norma {
namespace Python {
PyObject*
TrainingPair_to_python_tuple::convert(Norma::TrainingPair const& pair) {
    return bp::incref(bp::make_tuple(pair.source(), pair.target()).ptr());
}

void* TrainingData_from_python_list::convertible(PyObject* obj_ptr) {
    // can only convert lists
    if (PyList_Check(obj_ptr)) {
        Py_ssize_t size = PyList_Size(obj_ptr);
        for (Py_ssize_t i = 0; i < size; ++i) {
            // each list member must be a tuple
            PyObject* member = PyList_GetItem(obj_ptr, i);
            if (PyTuple_Check(member) && PyTuple_Size(member) == 2) {
                // each tuple must consist of exactly two strings
                PyObject* source = PyTuple_GetItem(member, 0);
                PyObject* target = PyTuple_GetItem(member, 1);
                if (!(PyString_Check(source) || PyUnicode_Check(source))
                    || !(PyString_Check(target) || PyUnicode_Check(target)))
                    return 0;
            } else {
                return 0;
            }
        }
    }
    return obj_ptr;
}

void TrainingData_from_python_list::construct(PyObject* obj_ptr,
                                              py_stage1_data* data) {
    // Grab pointer to memory into which to construct the new object
    void* storage = reinterpret_cast<py_storage*>(data)->storage.bytes;

    // Extract data from the python list
    Norma::TrainingData* training = new (storage) Norma::TrainingData();
    Py_ssize_t size = PyList_Size(obj_ptr);
    for (Py_ssize_t i = 0; i < size; ++i) {
        PyObject* member = PyList_GetItem(obj_ptr, i);
        PyObject* source = PyTuple_GetItem(member, 0);
        PyObject* target = PyTuple_GetItem(member, 1);
        string_impl si_source = bp::extract<string_impl>(source);
        string_impl si_target = bp::extract<string_impl>(target);
        training->add_pair(si_source, si_target);
    }

    data->convertible = storage;
}

void register_training_converters() {
    bp::to_python_converter<Norma::TrainingPair,
                            TrainingPair_to_python_tuple>();
    bp::converter::registry::push_back(
        &TrainingData_from_python_list::convertible,
        &TrainingData_from_python_list::construct,
        bp::type_id<Norma::TrainingData>());
}
}  // namespace Python
}  // namespace Norma
