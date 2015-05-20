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
// TODO(fpetran): find out what exactly PyThreadState is supposed to do
// this code is not exception safe: if a method throws while threadstate
// is swapped out, it may 1) leak the threadstate, and 2) try to delete
// the void threadstate
#include"external.h"
#include<Python.h>
#include<string>
#include<mutex>
#include<map>
#include<stdexcept>

namespace Norma {
namespace Normalizer {
namespace External {

External::External() : python_mutex(new std::mutex()) {
    Py_Initialize();
    PyEval_InitThreads();
    main_threadstate = PyThreadState_Get();
    interpreter_state = main_threadstate->interp;
    // i have honestly no idea what the threadstate does, but the internet
    // claims i need it and i need to swap it out before i use the interpreter.
    my_threadstate = PyThreadState_New(interpreter_state);
    // PyEval_InitThreads implicitly acquires lock
    PyEval_ReleaseLock();
}

External::~External() {
    if (_initialized)
        tear_down();
    PyEval_ReleaseLock();
    PyThreadState_Clear(my_threadstate);
    PyThreadState_Delete(my_threadstate);
    Py_Finalize();
}

void External::tear_down() {
    PyObject* pargs = PyTuple_New(0);
    PyObject_CallObject(teardown_fun, pargs);
    Py_DECREF(pargs);
    Py_XDECREF(script);
    Py_XDECREF(normalize_fun);
    Py_XDECREF(normalize_nbest_fun);
    Py_XDECREF(train_fun);
    Py_XDECREF(save_fun);
    Py_XDECREF(setup_fun);
    Py_XDECREF(teardown_fun);
}

void External::set_from_params(const std::map<std::string, std::string>&
                                                                       params) {
    _params = &params;
}

PyObject* External::get_function_ptr(const char* name) {
    std::string fun_name;
    if (_params->count(_name + name) == 0)
        fun_name = name;
    else
        fun_name = _params->at(_name + name);

    PyObject *fun = PyObject_GetAttrString(script, fun_name.c_str());
    if (fun == nullptr || !PyCallable_Check(fun)) {
        if (PyErr_Occurred())
            PyErr_Print();
        throw std::runtime_error("Function not found or not callable: "
                                 + fun_name);
    }
    Py_INCREF(fun);
    return fun;
}

namespace {
void set_path(const char* path_par) {
    PyObject *sys_path, *path;
    // if i use char* or the literal below, gcc will give a
    // deprecation warning, which i suspect is the fault of python
    char pn[5] = "path";
    sys_path = PySys_GetObject(pn);
    path = PyString_FromString(path_par);
    PyList_Append(sys_path, path);
    Py_DECREF(path);
}
}  // namespace

void External::init() {
    std::lock_guard<std::mutex> guard(*python_mutex);
    PyEval_AcquireLock();
    temp_state = PyThreadState_Swap(my_threadstate);
    if (_params->count(_name + ".path") != 0)
        set_path(_params->at(_name + ".path").c_str());

    PyObject *sname =
        PyString_FromString(_params->at(_name + ".script").c_str());
    script = PyImport_Import(sname);
    Py_DECREF(sname);
    if (script == nullptr)
        throw std::runtime_error
            (_params->at(_name +".script") + ".py not found!");

    if (_params->count(_name + ".name") != 0)
        _name = _params->at(_name + ".name");

    // set all function pointers
    normalize_fun = get_function_ptr("do_normalize");
    normalize_nbest_fun = get_function_ptr("do_normalize_nbest");
    train_fun = get_function_ptr("do_train");
    save_fun = get_function_ptr("do_save");
    setup_fun = get_function_ptr("do_setup");
    teardown_fun = get_function_ptr("do_teardown");

    // call setup function
    PyObject* pargs = PyTuple_New(0);
    PyObject_CallObject(setup_fun, pargs);
    Py_DECREF(pargs);
    my_threadstate = PyThreadState_Swap(temp_state);
    PyEval_ReleaseLock();
    _initialized = true;
}

Result External::do_normalize(const string_impl& word) const {
    std::lock_guard<std::mutex> guard(*python_mutex);
    PyEval_AcquireLock();
    temp_state = PyThreadState_Swap(my_threadstate);
    const char* word_cstr = to_cstr(word);
    PyObject *pargs = PyTuple_New(1);
    PyTuple_SetItem(pargs, 0, PyString_FromString(word_cstr));

    PyObject *result = PyObject_CallObject(normalize_fun, pargs);
    if (result == nullptr) {
        PyErr_Print();
        Py_DECREF(pargs);
        Py_DECREF(result);
        throw std::runtime_error("Python error");
    }
    string_impl cword = PyString_AsString(PyTuple_GetItem(result, 0));
    double score = PyFloat_AsDouble(PyTuple_GetItem(result, 1));

    Py_DECREF(pargs);
    Py_DECREF(result);
    my_threadstate = PyThreadState_Swap(temp_state);
    PyEval_ReleaseLock();
    return make_result(cword, score);
}

ResultSet External::do_normalize(const string_impl& word, unsigned int n) const {
    std::lock_guard<std::mutex> guard(*python_mutex);
    PyEval_AcquireLock();
    temp_state = PyThreadState_Swap(my_threadstate);
    const char* word_cstr = to_cstr(word);
    PyObject *pargs = PyTuple_New(2);
    PyTuple_SetItem(pargs, 0, PyString_FromString(word_cstr));
    PyTuple_SetItem(pargs, 1, PyInt_FromLong(n));

    PyObject* result = PyObject_CallObject(normalize_nbest_fun, pargs);
    if (result == nullptr) {
        PyErr_Print();
        Py_DECREF(pargs);
        Py_DECREF(result);
        throw std::runtime_error("Python error");
    }

    ResultSet resultset;
    string_impl cword;
    double score;
    auto result_size = PyList_Size(result);
    for (int i = 0; i < result_size; ++i) {
        PyObject* entry = PyList_GetItem(result, i);
        cword = PyString_AsString(PyTuple_GetItem(entry, 0));
        score = PyFloat_AsDouble(PyTuple_GetItem(entry, 1));
        resultset.push_back(make_result(cword, score));
        Py_DECREF(entry);
    }
    Py_DECREF(pargs);
    Py_DECREF(result);
    my_threadstate = PyThreadState_Swap(temp_state);
    PyEval_ReleaseLock();
    return resultset;
}

bool External::do_train(TrainingData* data) {
    std::lock_guard<std::mutex> guard(*python_mutex);
    PyEval_AcquireLock();
    temp_state = PyThreadState_Swap(my_threadstate);
    PyObject* pargs = PyTuple_New(0);
    PyObject_CallObject(train_fun, pargs);
    Py_DECREF(pargs);
    my_threadstate = PyThreadState_Swap(temp_state);
    PyEval_ReleaseLock();
    return true;
}

void External::do_save_params() {
    std::lock_guard<std::mutex> guard(*python_mutex);
    PyEval_AcquireLock();
    temp_state = PyThreadState_Swap(my_threadstate);
    PyObject* pargs = PyTuple_New(0);
    PyObject_CallObject(save_fun, pargs);
    Py_DECREF(pargs);
    my_threadstate = PyThreadState_Swap(temp_state);
    PyEval_ReleaseLock();
}

}  // namespace External
}  // namespace Normalizer
}  // namespace Norma

