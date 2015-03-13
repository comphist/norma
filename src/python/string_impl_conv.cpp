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
// This will register converters with Boost::Python that convert between
// ICU strings and python strings, and from PyUnicode strings to std::string.
// ----------------------------------------------------------------------------
#include"string_impl_conv.h"
#include<string>
#include<boost/python.hpp>  //NOLINT[build/include_order]
#include"string_impl.h"

namespace Norma {
namespace Python {

#ifdef USE_ICU_STRING

/* All of this is based on
   http://misspent.wordpress.com/2009/09/27/how-to-write-boost-python-converters/
*/

/// to-python conversion from ICU strings
PyObject* ICUString_to_python_str::convert(string_impl const& s) {
    std::string utf8;
    s.toUTF8String(utf8);
    PyObject* unicode = PyUnicode_FromString(utf8.c_str());
    return boost::python::incref(unicode);
}

/// determine if obj_ptr can be converted to an ICU string
void* ICUString_from_python_str::convertible(PyObject* obj_ptr) {
    if (!PyString_Check(obj_ptr)
        && !PyUnicode_Check(obj_ptr)) return 0;
    return obj_ptr;
}

/// from-python conversion to ICU strings
void ICUString_from_python_str::construct(PyObject* obj_ptr,
                                          py_stage1_data* data) {
    // Grab pointer to memory into which to construct the new ICU string
    void* storage = reinterpret_cast<py_storage*>(data)->storage.bytes;

    // Extract the character data from the python string
    if (PyUnicode_Check(obj_ptr)) {
        // UTF-8 encoding as an intermediate step for PyUnicode -> ICU
        const char* value = PyString_AsString(PyUnicode_AsUTF8String(obj_ptr));
        assert(value);
        new (storage) UnicodeString(UnicodeString::fromUTF8(value));
    } else {
        // we cannot know the proper encoding of byte strings in
        // Python 2.x, so we guess UTF-8
        const char* value = PyString_AsString(obj_ptr);
        assert(value);
        new (storage) UnicodeString(UnicodeString::fromUTF8(value));
    }

    // Stash the memory chunk pointer for later use by boost.python
    data->convertible = storage;
}

// register the converters
void register_string_impl_converters() {
    boost::python::to_python_converter<
        string_impl,
        ICUString_to_python_str>();

    boost::python::converter::registry::push_back(
        &ICUString_from_python_str::convertible,
        &ICUString_from_python_str::construct,
        boost::python::type_id<string_impl>());
}

#else  // USE_ICU_STRING

/// determine if obj_ptr is a latin1-encodable unicode string
void* STDString_from_python_unicode::convertible(PyObject* obj_ptr) {
    if (!PyUnicode_Check(obj_ptr)) return 0;
    if (!PyUnicode_AsLatin1String(obj_ptr)) return 0;
    return obj_ptr;
}

/// from-python conversion to std::string
void STDString_from_python_unicode::construct(PyObject* obj_ptr,
                                              py_stage1_data* data) {
    // Grab pointer to memory into which to construct the new std::string
    void* storage = reinterpret_cast<py_storage*>(data)->storage.bytes;

    // Extract the character data from the python string
    const char* value = PyString_AsString(PyUnicode_AsLatin1String(obj_ptr));
    assert(value);
    new (storage) string_impl(value);

    // Stash the memory chunk pointer for later use by boost.python
    data->convertible = storage;
}

// register the converters
void register_string_impl_converters() {
    boost::python::converter::registry::push_back(
        &STDString_from_python_unicode::convertible,
        &STDString_from_python_unicode::construct,
        boost::python::type_id<string_impl>());
}

#endif  // USE_ICU_STRING

}  // namespace Python
}  // namespace Norma
