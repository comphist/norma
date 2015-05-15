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
#include"lexicon.h"
#include<map>
#include<set>
#include<string>
#include<fstream>
#include<stdexcept>
#include<vector>
#include"exceptions.h"
#include"gfsm_wrapper.h"

namespace Norma {
namespace Normalizer {

const string_impl Lexicon::SYMBOL_BOUNDARY = "<#>";
const string_impl Lexicon::SYMBOL_ANY      = "<any>";
const string_impl Lexicon::SYMBOL_EPSILON  = "<eps>";

/***** INITIALIZATION *****/
void Lexicon::do_set_from_params(const std::map<std::string, std::string>&
                                 params) {
    if (params.count("Lexicon.fsmfile")) {
        _lexfile = boost::filesystem::path(params.at("Lexicon.fsmfile"));
        if (_lexfile.is_relative() && params.count("parent_path")) {
            _lexfile = boost::filesystem::path(params.at("parent_path"))
                / _lexfile;
        }
    } else if (params.count("perfilemode.input") != 0) {
        _lexfile = boost::filesystem::path(params.at("perfilemode.input"));
        _lexfile.replace_extension("Lexicon.fsmfile");
    }
    if (params.count("Lexicon.symfile")) {
        _symfile = params.at("Lexicon.symfile");
        if (_symfile.is_relative() && params.count("parent_path")) {
            _symfile = boost::filesystem::path(params.at("parent_path"))
                / _symfile;
        }
    } else if (params.count("perfilemode.input") != 0) {
        _lexfile = boost::filesystem::path(params.at("perfilemode.input"));
        _lexfile.replace_extension("Lexicon.symfile");
    }
}

void Lexicon::do_init() {
    if (_lexfile.empty() != _symfile.empty()) {
        throw std::runtime_error
            ("Error initializing lexicon: please specify either both fsmfile "
             "and symfile or none.");
    }

    do_clear();
    if (!_symfile.empty() && !boost::filesystem::exists(_symfile)) {
        throw init_error(
            "couldn't find lexicon symbol table: " + _symfile.string());
    } else if (boost::filesystem::exists(_lexfile)) {
        _fsm->load_binfile(_lexfile.string());
    } else if (!_lexfile.empty()) {
        throw init_error(
            "couldn't find lexicon automaton file: " + _lexfile.string());
    }
}

void Lexicon::do_clear() {
    if (_fsm != nullptr)
        delete _fsm;
    _fsm  = new Gfsm::StringAcceptor();
    _fsm->set_alphabet(init_alphabet());
    _fsm->ensure_root();
}

void Lexicon::do_save_params() {
    if (_fsm == nullptr)
        throw std::runtime_error("Tried to access uninitialized Lexicon");
    if (!_lexfile.empty())
        _fsm->save_binfile(_lexfile.string());
    if (!_symfile.empty())
        _fsm->get_alphabet().save_labfile(_symfile.string());
}

Gfsm::Alphabet Lexicon::init_alphabet() {
    Gfsm::Alphabet alph;
    if (boost::filesystem::exists(_symfile)) {
        alph.load_labfile(_symfile.string());
    }
    // make sure that special symbols are covered by the alphabet
    const std::vector<string_impl> required {Lexicon::SYMBOL_BOUNDARY,
                                             Lexicon::SYMBOL_ANY,
                                             Lexicon::SYMBOL_EPSILON};
    Gfsm::LabelVector v = alph.cover(required);
    _label_boundary = v.get(0);
    _label_any      = v.get(1);
    _label_epsilon  = v.get(2);
    return alph;
}

/********* ACCESS *********/
bool Lexicon::check_contains(const string_impl& word) const {
    if (_fsm == nullptr)
        throw std::runtime_error("Tried to access uninitialized Lexicon");
    std::vector<string_impl> vec;
    for (string_size i = 0; i < word.length(); ++i) {
        vec.push_back(from_char(word[i]));
    }
    vec.push_back(Lexicon::SYMBOL_BOUNDARY);
    return _fsm->accepts(vec);
}

bool Lexicon::check_contains_partial(const string_impl& word) const {
    if (_fsm == nullptr)
        throw std::runtime_error("Tried to access uninitialized Lexicon");
    if (word.length() == 0)
        return true;  // always accept empty word
    return _fsm->accepts(word);
}

bool Lexicon::add_word(const string_impl& word) {
    if (_fsm == nullptr)
        throw std::runtime_error("Tried to access uninitialized Lexicon");
    if (check_contains(word))
        return false;
    std::vector<string_impl> vec;
    for (string_size i = 0; i < word.length(); ++i) {
        if (word[i] == ' ' || word[i] == '\t')
            vec.push_back(Lexicon::SYMBOL_BOUNDARY);
        else
            vec.push_back(from_char(word[i]));
    }
    vec.push_back(Lexicon::SYMBOL_BOUNDARY);
    _fsm->add_word(vec, true);
    return true;
}

std::vector<string_impl> Lexicon::retrieve_all_entries() const {
    std::vector<string_impl> acc;
    if (_fsm == nullptr)
        return acc;
    std::set<std::vector<string_impl>> a = _fsm->accepted_vectors();
    for (const std::vector<string_impl>& v : a) {
        if (!v.empty() && v.back() == Lexicon::SYMBOL_BOUNDARY) {
            string_impl word;
            for (const string_impl& c : v) {
                if (c != Lexicon::SYMBOL_BOUNDARY) {
                    word += c;
                }
            }
            acc.push_back(word);
        }
    }
    return acc;
}

unsigned int Lexicon::get_size() const {
    // for now, we just get all entries and count them ...
    // (this is cached by LexiconInterface)
    return entries().size();
}

const Gfsm::Alphabet& Lexicon::get_alphabet() const {
    if (_fsm == nullptr)
        throw std::runtime_error("Tried to access uninitialized Lexicon");
    return _fsm->get_alphabet();
}

void Lexicon::optimize() {
    if (_fsm == nullptr)
        throw std::runtime_error("Tried to access uninitialized Lexicon");
    _fsm->arith_sr_zero_to_zero();
    _fsm->arcsort();
    _fsm->arcuniq();
    _fsm->determinize();
    // epsilon removal shouldn't be required for lexicon FSTs,
    // and setting this to true makes the minimization take AGES:
    _fsm->minimize(false);
}

}  // namespace Normalizer
}  // namespace Norma
