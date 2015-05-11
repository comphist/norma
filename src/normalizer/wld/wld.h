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
#ifndef NORMALIZER_WLD_WLD_H_
#define NORMALIZER_WLD_WLD_H_
#include<map>
#include<string>
#include<mutex>
#include<memory>
#include"gfsm_wrapper.h"
#include"string_impl.h"
#include"normalizer/base.h"
#include"normalizer/result.h"
#include"typedefs.h"
#include"weight_set.h"

namespace Norma {
namespace Normalizer {
class Lexicon;

namespace WLD {
class WLD : public Base {
 public:
     WLD();
     ~WLD();
     void set_from_params(const std::map<std::string, std::string>& params);
     void init();
     using Base::init;
     void clear();
     Result operator()(const string_impl& word) const;
     ResultSet operator()(const string_impl& word, unsigned int n) const;
     bool train(TrainingData* data);
     void save_params();

     void set_lexicon(LexiconInterface* lexicon);

     /// Get filename of the current parameter file
     const std::string& get_paramfile() const { return _paramfile; }
     /// Set filename for the parameter file
     WLD& set_paramfile(const std::string& paramfile) {
         _paramfile = paramfile;
         return *this;
     }
     /// Get length of n-grams used during training
     unsigned int get_train_ngrams() const { return _train_ngrams; }
     /// Set length of n-grams used during training
     WLD& set_train_ngrams(unsigned int n) {
         _train_ngrams = n;
         return *this;
     }
     /// Get final weight divisor used during training
     unsigned int get_train_divisor() const { return _train_divisor; }
     /// Set final weight divisor used during training
     WLD& set_train_divisor(unsigned int div) {
         _train_divisor = div;
         return *this;
     }
     /// Get maximum weight for normalization candidates (0 = no maximum)
     double get_maximum_weight() const { return _max_weight; }
     /// Set maximum weight for normalization candidates (0 = no maximum)
     WLD& set_maximum_weight(double w) {
         _max_weight = w;
         return *this;
     }
     /// Get maximum no. of operations during normalization (0 = no maximum)
     unsigned int get_maximum_ops() const { return _max_ops; }
     /// Set maximum no. of operations during normalization (0 = no maximum)
     WLD& set_maximum_ops(unsigned int ops) {
         _max_ops = ops;
         return *this;
     }

     void set_caching(bool value) const;
     void clear_cache() const;
     bool is_caching() const { return _caching; }

     /// trains on learned pairs
     bool perform_training();

 protected:
     Gfsm::StringCascade* _cascade = nullptr;
     Gfsm::StringTransducer* _wfst = nullptr;

 private:
     Gfsm::AutomatonBuilder& gfsm_builder
         = Gfsm::AutomatonBuilder::instance();
     std::string _paramfile;
     WeightSet _weights;
     TrainSet _pairs;
     unsigned int _train_ngrams = 3;
     unsigned int _train_divisor = 7;
     double _convergence_quota = 0.01;
     unsigned int _max_cycles = 20;
     unsigned int _max_ops = 0;
     double _max_weight = 0.0;
     Lexicon* _gfsm_lex = nullptr;

     mutable bool _caching = true;
     mutable std::map<string_impl, Result> _cache;
     mutable std::unique_ptr<std::mutex> cache_mutex;

     /// compiles FSTs for lookup
     void build_gfsm_objects();
     void compile_transducer();
     void compile_cascade();

     /// implements maximum weight heuristic (to make lookup faster)
     double determine_max_weight(const string_impl& word) const;
};
}  // namespace WLD
}  // namespace Normalizer
}  // namespace Norma

extern "C" Norma::Normalizer::Base* create_normalizer() {
    return new Norma::Normalizer::WLD::WLD;
}

extern "C" void destroy_normalizer(Norma::Normalizer::Base* n) {
    delete n;
}

#endif  // NORMALIZER_WLD_WLD_H_

