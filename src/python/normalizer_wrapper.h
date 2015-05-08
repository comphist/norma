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
#ifndef NORMA_PYTHON_NORMALIZER_WRAPPER_H_
#define NORMA_PYTHON_NORMALIZER_WRAPPER_H_
#include<string>
#include<boost/python.hpp>  //NOLINT[build/include_order]
#include"string_impl.h"
#include"training_data.h"
#include"normalizer/base.h"
#include"normalizer/result.h"
#include"lexicon/lexicon.h"

namespace Norma {
namespace Python {
namespace bp = boost::python;

/// a wrapper for normalizer functions inherited from Base
/** All normalizers inherit from Base, which is a pure virtual class.
 * This is supposedly difficult to model with Boost::Python, and
 * Python has duck typing anyway, hence I do the next best thing: make
 * a wrapper template that can be specialized for any normalizer, and
 * that defines all functions that a normalizer inherited from Base.
 **/
template <typename T>
struct base_wrapper {
    static bp::class_<T, boost::noncopyable>
        make_class(char const* name);

    static Norma::Normalizer::Lexicon& get_lexicon(T* t);
    static void set_lexicon(T* t, Norma::Normalizer::Lexicon* lexicon);
    static bool train(T* t, TrainingData data);
};

/// wrapper function for Base::get_lexicon
template <typename T>
Norma::Normalizer::Lexicon& base_wrapper<T>::get_lexicon(T* t) {
    Norma::Normalizer::Lexicon* lexicon =
        dynamic_cast<Norma::Normalizer::Lexicon*>(t->get_lexicon());
    return *lexicon;
}

/// wrapper function for Base::set_lexicon
template <typename T>
void base_wrapper<T>::set_lexicon(T* t, Norma::Normalizer::Lexicon* lexicon) {
    t->set_lexicon(lexicon);
}

/// wrapper function for Base::train
template <typename T>
bool base_wrapper<T>::train(T* t, TrainingData data) {
    return t->train(&data);
}

/// Make a Python class with a given name
template <typename T>
bp::class_<T, boost::noncopyable>
base_wrapper<T>::make_class(char const* name) {
    using Norma::TrainingData;
    using Norma::Normalizer::Result;
    using Norma::Normalizer::ResultSet;
    Result (T::*normalize_best)(const string_impl&) const = &T::operator();
    ResultSet (T::*normalize_n_best)(const string_impl&, unsigned int) const
        = &T::operator();
    bp::docstring_options local_docstring_options(true, true, false);

    return bp::class_<T, boost::noncopyable>(name)
        .def("init", static_cast<void(T::*)()>(&T::init),
             "Initialize the normalizer.\n\n"
             "Loads data from any parameter files that have previously been "
             "set, and creates all necessary internal objects."
             )
        .def("clear", &T::clear,
             "Clear all trained parametrizations.\n\n"
             "Returns the normalizer to a clean, untrained state.  "
             "All other settings (e.g., filenames, lexicon) are left intact."
             )
        .def("save", &T::save_params,
             "Save the normalizer state to a file.\n\n"
             "Saves the current state of the normalizer to any "
             "previously set parameter file(s)."
             )
        .def("normalize", normalize_best,
             "Normalize a word and return the best candidate.\n\n"
             "Arguments:\n"
             "  word -- The string that should be normalized\n\n"
             "Returns:\n"
             "  A tuple of the form (<normalized_word>, <score>, self.name)"
             )
        .def("normalize", normalize_n_best,
             "Normalize a word and return the n best candidates.\n\n"
             "Arguments:\n"
             "  word -- The string that should be normalized\n"
             "     n -- How many candidates to return\n\n"
             "Returns:\n"
             "  A list of up to n tuples (<normalized_word>, <score>, "
             " self.name) in descending order of <score>.  The number of"
             " returned tuples can be less than n if the normalizer couldn't"
             " find more candidates."
             )
        .def("__call__", normalize_best, "Alias for normalize(word)")
        .def("__call__", normalize_n_best, "Alias for normalize(word, n)")
        .def("train", &base_wrapper<T>::train,
             "Train the normalizer on a list of word forms.\n\n"
             "Arguments:\n"
             "  data -- A list of tuples (<input_word>, <normalized_word>)"
             )
        .add_property("lexicon",
                      bp::make_function(&base_wrapper<T>::get_lexicon,
                                        bp::return_internal_reference<>()),
                      bp::make_function(&base_wrapper<T>::set_lexicon,
                                        bp::with_custodian_and_ward<1, 2>()),
                      "A lexicon used to restrict the list of possible "
                      "normalization candidates."
                      )
        .add_property("name", &T::name);
}

/// a wrapper for the specific normalizers
struct normalizer_wrapper {
    template <typename T, T& (T::*SetParameterFile)(const std::string&)>
    static void init(T* t, std::string paramfile,
                     Norma::Normalizer::Lexicon* lexicon);
    template <typename T, T& (T::*SetParameterFile)(const std::string&)>
    static void init_nolex(T* t, std::string paramfile);
    template <typename T, T& (T::*SetParameterFile)(const std::string&)>
    static void save(T* t, std::string paramfile);

    static void wrap_Mapper();
    static void wrap_Rulebased();
    static void wrap_WLD();
};

/// wrapper functions for convenient "init" overloads which set
/// the parameter file (and optionally a lexicon) and call init()
/// at the same time
/* template takes the normalizer class and a function pointer to the
   function which sets the parameter, which has different names for
   different normalizers */
template <typename T, T& (T::*SetParameterFile)(const std::string&)>
void normalizer_wrapper::init(T* t, std::string paramfile,
                              Norma::Normalizer::Lexicon* lexicon) {
    t->set_lexicon(lexicon);
    (t->*SetParameterFile)(paramfile);
    t->init();
}

template <typename T, T& (T::*SetParameterFile)(const std::string&)>
void normalizer_wrapper::init_nolex(T* t, std::string paramfile) {
    (t->*SetParameterFile)(paramfile);
    t->init();
}

template <typename T, T& (T::*SetParameterFile)(const std::string&)>
void normalizer_wrapper::save(T* t, std::string paramfile) {
    (t->*SetParameterFile)(paramfile);
    t->save_params();
}
}  // namespace Python
}  // namespace Norma

#endif  // NORMA_PYTHON_NORMALIZER_WRAPPER_H_
