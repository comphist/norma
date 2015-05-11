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
#ifndef APPLICATOR_H_
#define APPLICATOR_H_
#include<list>
#include<string>
#include<map>
#include<functional>
#include"string_impl.h"
#include"normalizer/result.h"
#include"normalizer/base.h"

namespace Norma {
class TrainingData;

namespace Normalizer {
class LexiconInterface;
class Base;
}  // namespace Normalizer

/// Apply different Normalizers
/** This class is holds a chain of *Normalizer, and the register of
 *  Normalizer objects. In the future, it should also provide means
 *  of selecting between the results of the Normalizer Result s, which
 *  is currently only based on score returned.
 *
 *  It also takes care of concurrency for normalizing and training,
 *  in that all Normalizer methods are run in parallel, and also
 *  all Normalizer train() methods are run in parallel.
 *
 *  The reasoning behind this is to hide all implementation of the
 *  Normalizer s from the main Cycle, to hide the Cycle logic
 *  from the Normalizer, and most importantly to hide as many
 *  concurrency related issues as possible from the authors of
 *  the Normalizer.
 **/
class Applicator : private std::list<Normalizer::Base*> {
 public:
     explicit Applicator(const std::string& chain_definition,
                         const std::string& plugin_base_param,
                         const std::map<std::string, std::string>& params);
     Applicator() = delete;
     Applicator(const Applicator& a) = delete;
     const Applicator& operator=(const Applicator& a) = delete;
     ~Applicator();

     /// add a normalizer to the back of the chain
     inline void push_chain(Normalizer::Base* n);
     /// start all normalizers for a word in parallel
     /// then select the best result as soon as all are
     /// ready
     Normalizer::Result normalize(const string_impl& word) const;
     /// start all training in parallel, then wait until
     /// all of them are finished. Also lock a Normalizers
     /// mutex before the training start, because its internal
     /// state will be messed up if we train twice on different
     /// history.
     void train(TrainingData *data);
     /// choose between two results
     std::function<const Normalizer::Result&(const Normalizer::Result&,
                                             const Normalizer::Result&)>
                   chooser = Applicator::best_priority;
     /// choose the result with the best score
     static const Normalizer::Result& best_score(const Normalizer::Result& one,
                                                 const Normalizer::Result& two);
     /// choose the result with the lowest priority and score > 0
     static const Normalizer::Result&
         best_priority(const Normalizer::Result& one,
                       const Normalizer::Result& two);
     void save_params();
     void init_chain();

 private:
     Normalizer::Base* create_plugin(const std::string& lib_name,
                                     const std::string& alias = "");
     std::list<std::pair<destroy_t*, Normalizer::Base*>> created_normalizers;
     std::list<void*> loaded_plugins;
     const std::map<std::string, std::string>& config_vars;
     std::string chain_def, plugin_base = ".";
     Normalizer::LexiconInterface* _lex;
};
}  // namespace Norma

#endif  // APPLICATOR_H_

