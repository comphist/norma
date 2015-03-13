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
#ifndef NORMALIZER_EXTERNAL_EXTERNAL_H_
#define NORMALIZER_EXTERNAL_EXTERNAL_H_
#include<Python.h>
#include<map>
#include<string>
#include<memory>
#include<mutex>
#include"base.h"
#include"result.h"

namespace Norma {
namespace Normalizer {
namespace External {

/** An external normalizer implemented in Python.
 *  cfg file parameters:
 *  script: name of the main .py file (without extension)
 *  path (optional): path to the implementation file
 *  name (optional): name of the normalizer
 *                   this is used to distinguish different normalizers
 *                   later, and it cannot be a normalizer that already
 *                   exists. you want to set this if you have several
 *                   python normalizers. it defaults to "External".
 *  following are all optional, they default to the key name:
 *  do_normalize: normalization function
 *                needs to take string arg and return tuple (string, float)
 *  do_normalize_nbest: normalization function returning n best results
 *                      needs to take string arg and return list of tuples
 *                      norma will assume that the first item in the list is
 *                      the best normalization
 *  do_train: training function
 *  do_save: save parameters
 *  do_setup: setup function to be called once at the initialization
 *            of the normalizer, can be used to setup shared resources etc
 *  do_teardown: teardown function to be called once at the destruction of
 *               the normalizer
 *               NOTE: the setup function cannot be guaranteed to have been
 *                     called when do_teardown is called!
 **/
class External : public Base {
 public:
     External();
     ~External();
     void init();
     using Base::init;
     void set_from_params(const std::map<std::string, std::string>& params);
     Result operator()(const string_impl& word) const;
     ResultSet operator()(const string_impl& word, unsigned int n) const;
     bool train(TrainingData* data);
     void save_params();
     const char* name() const { return _name.c_str(); }

 private:
     /// clean up stuff that was set in initialization.
     void tear_down();
     std::string _name = "External";
     std::unique_ptr<std::mutex> python_mutex;
     const std::map<std::string, std::string>* _params;
     bool _initialized = false;

     PyObject* get_function_ptr(const char* name);
     // if i initialize these to nullptr, norma segfaults
     // on entering ~Base(). why? nobody knows
     PyObject *script,
              *setup_fun, *teardown_fun,
              *normalize_fun,
              *normalize_nbest_fun,
              *train_fun,
              *save_fun;
     mutable PyThreadState *my_threadstate, *temp_state;
     PyThreadState *main_threadstate;
     PyInterpreterState *interpreter_state;
};

}  // namespace External
}  // namespace Normalizer
}  // namespace Norma

#endif  // NORMALIZER_EXTERNAL_EXTERNAL_H_

