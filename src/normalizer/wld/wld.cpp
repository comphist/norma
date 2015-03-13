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
#include"wld.h"
#include<map>
#include<set>
#include<string>
#include<sstream>
#include<tuple>
#include<vector>
#include<cmath>
#include<stdexcept>
#include"result.h"
#include"gfsm_wrapper.h"
#include"string_impl.h"
#include"iobase.h"
#include"levenshtein_aligner.h"
#include"lexicon.h"
#include"symbols.h"
#include"weight_set.h"

namespace Norma {
namespace Normalizer {
namespace WLD {
namespace {
double calculate_probability(double weight) {
    return std::exp(-weight);
}

void strip_boundary_symbol(std::vector<string_impl>* vec) {
    if (vec->back() == Lexicon::SYMBOL_BOUNDARY)
        vec->pop_back();
}
}  // namespace

WLD::WLD() : cache_mutex(new std::mutex()) {}

void WLD::set_from_params(const std::map<std::string, std::string>& params) {
    if (params.count("WLD.paramfile") != 0) {
        set_paramfile(to_absolute(params.at("WLD.paramfile"), params));
    } else if (params.count("perfilemode.input") != 0) {
        set_paramfile(with_extension(params.at("perfilemode.input"),
                                     "WLD.paramfile"));
    }
    if (params.count("WLD.train_ngrams") != 0) {
        std::stringstream ss;
        unsigned int n;
        ss << params.at("WLD.train_ngrams");
        if (ss >> n)
            set_train_ngrams(n);
    }
    if (params.count("WLD.train_divisor") != 0) {
        std::stringstream ss;
        unsigned int div;
        ss << params.at("WLD.train_divisor");
        if (ss >> div)
            set_train_divisor(div);
    }
    if (params.count("WLD.max_weight") != 0) {
        std::stringstream ss;
        double w;
        ss << params.at("WLD.max_weight");
        if (ss >> w)
            set_maximum_weight(w);
    }
    if (params.count("WLD.max_ops") != 0) {
        std::stringstream ss;
        unsigned int ops;
        ss << params.at("WLD.max_ops");
        if (ss >> ops)
            set_maximum_ops(ops);
    }
}

WLD::~WLD() {
    if (_wfst != nullptr)
        delete _wfst;
    if (_cascade != nullptr)
        delete _cascade;
    // do not delete _gfsm_lex
}

void WLD::init() {
    clear();
    if (!_paramfile.empty())
        _weights.read_paramfile(_paramfile);
    build_gfsm_objects();
}

void WLD::clear() {
    clear_cache();
    _weights.clear();
    _pairs.clear();
    if (_wfst != nullptr) {  // TODO(bollmann): maybe change this to empty init?
        delete _wfst;
        _wfst = nullptr;
    }
    if (_cascade != nullptr) {
        delete _cascade;
        _cascade = nullptr;
    }
}

void WLD::set_lexicon(LexiconInterface* lexicon) {
    Lexicon* gfsm_lex = dynamic_cast<Lexicon*>(lexicon);
    if (!gfsm_lex)
        throw std::runtime_error("WLD normalizer requires "
                                 "Gfsm lexicon implementation!");
    _lex = lexicon;
    _gfsm_lex = gfsm_lex;
}

Result WLD::operator()(const string_impl& word) const {
    if (_caching && _cache.count(word) > 0)
        return _cache.at(word);

    ResultSet resultset = this->operator()(word, 1);
    Result result;
    if (resultset.size() == 0)
        result = make_result(word, 0.0);
    else
        result = resultset.front();

    if (_caching) {
        std::lock_guard<std::mutex> guard(*cache_mutex);
        _cache[word] = result;
    }

    return result;
}

ResultSet WLD::operator()(const string_impl& word, unsigned int n) const {
    if (_cascade == nullptr || _gfsm_lex == nullptr)
        return ResultSet();

    // can't do this in set_maximum_ops because cascade might not exist yet:
    if (_max_ops > 0)
        _cascade->set_max_ops(_max_ops);

    auto results = _cascade->lookup_nbest(word, n, determine_max_weight(word));
    if (results.size() == 0)
        return ResultSet();

    // convert cascade output to Result
    ResultSet resultset;
    for (auto stringpath : results) {
        std::vector<string_impl> output = stringpath.output;
        strip_boundary_symbol(&output);
        resultset
            .push_back(make_result(Gfsm::implode(output),
                                   calculate_probability(stringpath.weight)));
        resultset.back().origin = std::string(name());
    }
    return resultset;
}

bool WLD::train(TrainingData* data) {
    for (auto pp = data->rbegin(); pp != data->rend(); ++pp) {
        if (pp->is_used())
            break;
        auto pair = std::make_pair(pp->source(), pp->target());
        if (_pairs.count(pair) > 0) {
            ++(_pairs[pair]);
        } else {
            _pairs[pair] = 1;
        }
    }
    return true;
}

void WLD::save_params() {
    perform_training();  // TODO(bollmann): this is a hack
    _weights.save_paramfile(_paramfile);
}

void WLD::set_caching(bool value) const {
    std::lock_guard<std::mutex> guard(*cache_mutex);
    _caching = value;
    if (!value)
        _cache.clear();
}

void WLD::clear_cache() const {
    std::lock_guard<std::mutex> guard(*cache_mutex);
    _cache.clear();
}

void WLD::compile_transducer() {
    // initialize objects
    _wfst = new Gfsm::StringTransducer(gfsm_builder.make_stringtransducer(
                                                Gfsm::SemiringType::TROPICAL));
    Gfsm::Alphabet alph_in(_gfsm_lex->get_alphabet()),
                   alph_out(_gfsm_lex->get_alphabet());
    alph_in.cover(_weights.input_symbols());
    alph_in.add_symbol(Symbols::ANY);
    alph_in.set_unknown_mapper(&Symbols::map_to_any);
    alph_out.add_symbol(Symbols::ANY);
    alph_out.set_unknown_mapper(&Symbols::map_to_any);
    _wfst->set_input_alphabet(alph_in);
    _wfst->set_output_alphabet(alph_out);

    // perform the actual compilation
    using Gfsm::StringPath;
    std::set<string_impl> input_symbols  = alph_in.covered(),
                          output_symbols = alph_out.covered();
    double id_cost  = _weights.default_identity_cost(),
           rep_cost = _weights.default_replacement_cost(),
           del_cost = _weights.default_deletion_cost();
    for (const auto& w : _weights.weight_map()) {  // customized weights
        StringPath path(w.first.first, w.first.second, w.second);
        _wfst->add_cyclic_path(path, false);
    }
    for (const auto& symi : input_symbols) {  // default weights
        // identity & deletion
        _wfst->add_cyclic_path(StringPath({symi}, {symi}, id_cost), false);
        _wfst->add_cyclic_path(StringPath({symi}, {}, del_cost), false);
        for (const auto& symo : output_symbols) {  // replacement
            _wfst->add_cyclic_path(StringPath({symi}, {symo}, rep_cost), false);
        }
    }
    // if we compose with the lexicon, final character has to be word boundary
    _wfst->add_path(Gfsm::StringPath({}, {Lexicon::SYMBOL_BOUNDARY}, 0.0));
}

void WLD::compile_cascade() {
    _cascade = new Gfsm::StringCascade(gfsm_builder.make_stringcascade(2,
                                                Gfsm::SemiringType::TROPICAL));
    _cascade->append(*_wfst);
    _cascade->append(_gfsm_lex);
    _cascade->sort();
}

void WLD::build_gfsm_objects() {
    if (_wfst != nullptr)
        delete _wfst;
    if (_cascade != nullptr)
        delete _cascade;
    if (!_weights.empty() && _gfsm_lex != nullptr) {
        compile_transducer();
        compile_cascade();
    }
}

bool WLD::perform_training() {
    LevenshteinAligner levenshtein(_weights, _train_ngrams, _train_divisor);
    unsigned int cycles = 0;
    do {
        levenshtein.perform_training_cycle(_pairs);
    } while (levenshtein.meandiff() > _convergence_quota
             && ++cycles < _max_cycles);
    _weights = levenshtein.make_final_weight_set(_pairs);
    // rebuild objects
    build_gfsm_objects();
    clear_cache();
    return true;
}

double WLD::determine_max_weight(const string_impl& word) const {
    if (_max_weight > 0)
        return _max_weight;
    else
        return 2.0 * word.length() * _weights.default_replacement_cost();
}
}  // namespace WLD
}  // namespace Normalizer
}  // namespace Norma
