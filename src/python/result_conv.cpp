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
#include<sstream>
#include<string>
#include<boost/python.hpp>  //NOLINT[build/include_order]
#include"string_impl.h"
#include"normalizer/result.h"

namespace bp = boost::python;
using Norma::Normalizer::Result;
using Norma::Normalizer::ResultSet;

namespace Norma {
namespace Python {
void* Result_from_python_tuple::convertible(PyObject* obj_ptr) {
    // can only convert tuples
    if (!PyTuple_Check(obj_ptr))
        return 0;
    // tuple must be (str, float) or (str, float, str)
    Py_ssize_t size = PyTuple_Size(obj_ptr);
    if (size < 2 || size > 3)
        return 0;
    PyObject* result_word = PyTuple_GetItem(obj_ptr, 0);
    PyObject* result_score = PyTuple_GetItem(obj_ptr, 1);
    if (!(PyString_Check(result_word) || PyUnicode_Check(result_word))
        || !(PyFloat_Check(result_score) || PyInt_Check(result_score)))
        return 0;
    if (size == 3) {
        PyObject* result_origin = PyTuple_GetItem(obj_ptr, 2);
        if (!(PyString_Check(result_origin) || PyUnicode_Check(result_origin)))
            return 0;
    }
    return obj_ptr;
}

void Result_from_python_tuple::construct(PyObject* obj_ptr,
                                         py_stage1_data* data) {
    // Grab pointer to memory into which to construct the new object
    void* storage = reinterpret_cast<py_storage*>(data)->storage.bytes;

    // Extract data from the python tuple
    PyObject* ptr_word = PyTuple_GetItem(obj_ptr, 0);
    PyObject* ptr_score = PyTuple_GetItem(obj_ptr, 1);
    // .word
    string_impl result_word = bp::extract<string_impl>(ptr_word);
    // .score
    double result_score;
    if(PyFloat_Check(ptr_score))
        result_score = PyFloat_AsDouble(ptr_score);
    else
        result_score = PyInt_AsLong(ptr_score);
    // .origin
    Result* result = new (storage) Result(result_word, result_score);
    if(PyTuple_Size(obj_ptr) > 2) {
        PyObject* ptr_origin = PyTuple_GetItem(obj_ptr, 2);
        std::string result_origin = bp::extract<std::string>(ptr_origin);
        result->origin = result_origin;
    }

    data->convertible = storage;
}

PyObject* ResultSet_to_python_list::convert(ResultSet const& rs) {
    bp::list l;
    for (Result const& r : rs) {
        l.append(r);
    }
    return bp::incref(l.ptr());
}

void register_result_converters() {
    bp::converter::registry::push_back(
        &Result_from_python_tuple::convertible,
        &Result_from_python_tuple::construct,
        bp::type_id<Result>());

    bp::to_python_converter<ResultSet, ResultSet_to_python_list>();
}

string_impl result_wrapper::repr(const Result& result) {
    std::ostringstream ss;
    ss << "Result('" << result.word << "', " << result.score
       << ", '" << result.origin << "')";
    std::string str(ss.str());
    string_impl my_repr(str.c_str());
    return my_repr;
}

void result_wrapper::wrap() {
    bp::class_<Result>("Result")
        .def(bp::init<string_impl, double>())
        .def(bp::init<string_impl, double, char*>())
        .add_property("word",
                      bp::make_getter(
                          &Result::word,
                          bp::return_value_policy<bp::return_by_value>()
                      ),
                      "The normalized word."
                     )
        .add_property("score",
                      bp::make_getter(&Result::score),
                      "The normalizer's score for this result."
                     )
        .add_property("origin",
                      bp::make_getter(
                          &Result::origin,
                          bp::return_value_policy<bp::return_by_value>()
                      ),
                      bp::make_setter(&Result::origin),
                      "Name of the normalizer that produced this result."
                     )
        .def("__eq__", &Result::operator==)
        .def("__neq__", &Result::operator!=)
        .def("__lt__", &Result::operator<)
        .def("__gt__", &Result::operator>)
        .def("__repr__", &result_wrapper::repr)
        ;
}
}  // namespace Python
}  // namespace Norma
