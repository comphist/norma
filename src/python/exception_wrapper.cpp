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
#include"exception_wrapper.h"
#include<string>
#include<boost/python.hpp>  //NOLINT[build/include_order]
#include"normalizer/exceptions.h"

namespace bp = boost::python;
using Norma::Normalizer::init_error;

namespace Norma {
namespace Python {

PyObject *PyExc_Norma_InitError = NULL;

void translate_norma_exception(const init_error& e) {
    assert(PyExc_Norma_InitError != NULL);
    PyErr_SetString(PyExc_Norma_InitError, e.what());
}

void register_exception_translators() {
    bp::class_<init_error> Norma_InitError
        ("NormaInitError", bp::init<std::string>());
    PyExc_Norma_InitError = Norma_InitError.ptr();
    bp::register_exception_translator<init_error>(&translate_norma_exception);
}

}  // namespace Python
}  // namespace Norma
