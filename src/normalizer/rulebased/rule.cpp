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
#include"rule.h"
#include<iostream>
#include<sstream>
#include<string>
#include<stdexcept>
#include<vector>
#include<list>
#include<tuple>

namespace Norma {
namespace Normalizer {
namespace Rulebased {

namespace {

string_impl combine_rule_parts(string_impl r1, string_impl r2) {
    if (r1 == Symbols::EPSILON && r2 == Symbols::EPSILON)
        return Symbols::EPSILON;
    else if (r1 == Symbols::EPSILON)
        return r2;
    else if (r2 == Symbols::EPSILON)
        return r1;
    else
        return r1 + r2;
    throw std::runtime_error("Something horrible happened.");
}

}  // namespace

/////////////////////// Rule //////////////////////////////////////////////////
Rule::Rule(EditOp type,
     const string_impl& source, size_t spos,
     const string_impl& target, size_t tpos) {
    _spos = spos;
    _tpos = tpos;
    _from = (type == EditOp::ADD) ? Symbols::EPSILON[0] : source[spos-1];
    _to   = (type == EditOp::DEL) ? Symbols::EPSILON[0] : target[tpos-1];
}

bool Rule::matches_left(const char_impl& left) const {
    return left == _left;
}

bool Rule::matches_right(const char_impl& right) const {
    return right == _right;
}

bool Rule::matches_back(const string_impl& back, bool eps) const {
    if (eps != (_from == Symbols::EPSILON))
        return false;
    if (eps)
        return matches_right(back[0]);
    string_impl rule_back = _from + from_char(_right);
    string_impl back_substr;
    extract(back, 0, rule_back.length(), &back_substr);
    return (back_substr == rule_back);
}

bool Rule::operator==(const Rule& that) const {
    return
        _from == that._from &&
        _to == that._to &&
        _left == that._left &&
        _right == that._right;
}

bool Rule::operator<(const Rule& that) const {
    // lexicographical ordering of the members variables,
    // lazily implemented by falling back to the STL algorithms
    return (std::make_tuple(_from, _to, _left, _right)
            < std::make_tuple(that._from, that._to, that._left, that._right));
}

/////////////////////// RuleSet ///////////////////////////////////////////////
RuleSet::RuleSet() {
    _rules = std::vector<Rule>();
}

RuleSet::RuleSet(const RuleSet& that) {
    _rules = std::vector<Rule>();
    for (auto rule = that.cbegin(); rule != that.cend(); ++rule)
        _rules.push_back(Rule(*rule));
}

const RuleSet& RuleSet::operator=(const RuleSet& that) {
    _rules.clear();
    _rules = std::vector<Rule>();
    for (auto rule = that.cbegin(); rule != that.cend(); ++rule)
        _rules.push_back(Rule(*rule));
    return *this;
}

int RuleSet::cost() {
    int c = 0;
    for (auto rule : _rules)
        c += rule.cost();
    return c;
}

int RuleSet::count(EditOp type) {
    int c = 0;
    for (auto rule : _rules)
        if (rule.type() == type)
            ++c;
    return c;
}

void RuleSet::add_rule(EditOp type,
                       const string_impl& source, size_t spos,
                       const string_impl& target, size_t tpos) {
    Rule r(type, source, spos, target, tpos);
    r._left  = left_context();
    r._right = Symbols::BOUNDARY;
    if (_rules.size() > 0 && r.from() != Symbols::EPSILON)
        _rules.back()._right = r.from()[0];
    _rules.push_back(r);
}

void RuleSet::add_rule(const Rule& rule) {
    _rules.push_back(rule);
}

char_impl RuleSet::left_context() {
    if (_rules.size() == 0)
        return Symbols::BOUNDARY;
    if (_rules.back().to() != Symbols::EPSILON) {
        string_size last = _rules.back().to().length() - 1;
        return _rules.back().to()[last];
    }

    int i = _rules.size() - 1;
    while (--i > 0)
        if (_rules[i].to() != Symbols::EPSILON) {
            string_size last = _rules[i].to().length() - 1;
            return _rules[i].to()[last];
        }

    return Symbols::BOUNDARY;
}

void RuleSet::delete_positions(const std::list<int>& positions) {
    int deletions = 0;
    for (int pos : positions) {
        _rules.erase(_rules.begin() + (pos - deletions));
        ++deletions;
    }
}

void RuleSet::merge_same_coordinate() {
    std::list<int> delete_pos;
    for (unsigned int i = 1; i < _rules.size(); ++i) {
        Rule& previous = _rules[i - 1];
        if ((previous.spos() == _rules[i].spos()
          || previous.tpos() == _rules[i].tpos())
         && (previous.type() != EditOp::IDENT
          && _rules[i].type() != EditOp::IDENT)) {
            previous._from  = combine_rule_parts(previous.from(),
                                                 _rules[i].from());
            previous._to    = combine_rule_parts(previous.to(),
                                                 _rules[i].to());
            previous._right = _rules[i].right();
            delete_pos.push_back(i);
        }
    }

    delete_positions(delete_pos);
}

void RuleSet::merge_neighbors() {
    std::list<int> delete_pos;
    for (unsigned int i = 1; i < _rules.size(); ++i)
        if (_rules[i].type() != EditOp::IDENT) {
            Rule& candidate = _rules[i-1];
            if (candidate.type() != EditOp::IDENT
             && (candidate.spos() == _rules[i].spos() - 1
              || candidate.tpos() == _rules[i].tpos() - 1)) {
                _rules[i]._from = combine_rule_parts(candidate.from(),
                                                    _rules[i].from());
                _rules[i]._to   = combine_rule_parts(candidate.to(),
                                                    _rules[i].to());
                _rules[i]._left = candidate.left();
                delete_pos.push_back(i - 1);
            }
        }

    delete_positions(delete_pos);
}

void RuleSet::insert_epsilon_identity(const string_impl& source,
                                      const string_impl& target) {
    std::vector<Rule> rules_new;
    // front boundary epsilon
    if (_rules[0].type() != EditOp::ADD)
        rules_new.push_back(Rule(Symbols::EPSILON, Symbols::EPSILON,
                                 Symbols::BOUNDARY, source[0]));

    rules_new.push_back(_rules[0]);
    for (unsigned int i = 1; i < _rules.size(); ++i) {
        if (_rules[i].from() != Symbols::EPSILON
            && _rules[i - 1].from() != Symbols::EPSILON) {
            Rule eps(EditOp::IDENT,
                     source, _rules[i].spos(),
                     target, _rules[i].tpos());
            eps._from = Symbols::EPSILON;
            eps._to   = Symbols::EPSILON;
            int j = i - 1;
            while (_rules[j].to() == Symbols::EPSILON)
                --j;
            eps._left = (j < 0)
                ? Symbols::BOUNDARY
                : _rules[j].to()[_rules[j].to().length() - 1];
            eps._right = _rules[i].from()[0];
            rules_new.push_back(eps);
        }
        rules_new.push_back(_rules[i]);
    }

    // back boundary epsilon
    if (_rules.back().type() != EditOp::ADD)
        rules_new.push_back(Rule(Symbols::EPSILON, Symbols::EPSILON,
                                 target[target.length() - 1],
                                 Symbols::BOUNDARY));

    _rules = rules_new;
}

std::size_t RuleHasher::operator()(const Rule& r) const {
    std::stringstream ss;
    std::string s;
    ss << r;
    ss >> s;
    return std::hash<std::string>()(s);
}

std::ostream& operator<<(std::ostream& strm,
                         const Norma::Normalizer::Rulebased::Rule& r) {
    strm << "{" << r.from() << "->" << r.to()
         << "/" << from_char(r.left()) << "_" << from_char(r.right()) << "}";
    return strm;
}

std::ostream& operator<<(std::ostream& strm,
                         const Norma::Normalizer::Rulebased::RuleSet& rs) {
    for (auto rule = rs.cbegin(); rule != rs.cend(); ++rule)
        strm << *rule << std::endl;
    return strm;
}
}  // namespace Rulebased
}  // namespace Normalizer
}  // namespace Norma
