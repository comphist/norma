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
#ifndef NORMALIZER_RULEBASED_RULE_H_
#define NORMALIZER_RULEBASED_RULE_H_
#include<list>
#include<vector>
#include<iosfwd>
#include<stdexcept>
#include"string_impl.h"
#include"symbols.h"

namespace Norma {
namespace Normalizer {
namespace Rulebased {
class RuleSet;
class Rulebased;

enum class EditOp {
    ADD,
    DEL,
    SUB,
    IDENT
};

/// A Chomsky-Halle style character rewrite rule
class Rule {
    friend class RuleSet;
    friend class RuleCollection;
 public:
     /// ctor for use when constructing the initial matrix
     Rule(EditOp type,
          const string_impl& source, size_t spos,
          const string_impl& target, size_t tpos);
     /// ctor for use when constructing a rule from scratch
     Rule(const string_impl& from, const string_impl& to,
          const char_impl& lc, const char_impl& rc)
     : _from(from), _to(to), _left(lc), _right(rc) {}
     /// convenience ctor where all arguments have the same type
     Rule(const string_impl& from, const string_impl& to,
          const string_impl& lc, const string_impl& rc)
     : _from(from), _to(to), _left(lc[0]), _right(rc[0]) {
         if (lc.length() > 1 || rc.length() > 1)
             throw std::invalid_argument
                 ("Contexts cannot be longer than one character.");
     }

     // getters
     inline const string_impl& from() const { return _from; }
     inline const string_impl& to() const { return _to; }
     inline const char_impl& left() const { return _left; }
     inline const char_impl& right() const { return _right; }

     /// return the edit operation of this rule
     inline EditOp type() const {
         // this should probably be cached or something
         if (_to == _from)
             return EditOp::IDENT;
         if (_to == Symbols::EPSILON)
             return EditOp::DEL;
         if (_from == Symbols::EPSILON)
             return EditOp::ADD;
         return EditOp::SUB;
     }
     /// the cost for non-identity rules can be adjusted here
     inline int cost() const {
         return (type() == EditOp::IDENT) ? 0 : 1;
     }

     // check if this rule is applicable given various contexts
     bool matches_left(const char_impl& left) const;
     bool matches_right(const char_impl& right) const;
     bool matches_back(const string_impl& back, bool eps) const;

     bool operator==(const Rule& that) const;
     inline bool operator!=(const Rule& that) const {
         return !(*this == that);
     }
     bool operator<(const Rule& that) const;

 protected:
     Rule() = default;
     inline size_t tpos() const { return _tpos; }
     inline size_t spos() const { return _spos; }
     string_impl _from  = Symbols::EPSILON,
                 _to    = Symbols::EPSILON;
     size_t _tpos = 0,
            _spos = 0;
     char_impl   _left  = Symbols::BOUNDARY,
                 _right = Symbols::BOUNDARY;
};

/// A set of rules
class RuleSet {
     friend RuleSet learn_rules(const string_impl&, const string_impl&,
                            bool, bool);

 public:
     // std container facade
     typedef std::vector<Rule>::const_iterator const_iterator;
     inline const_iterator cbegin() const { return _rules.cbegin(); }
     inline const_iterator cend() const { return _rules.cend(); }
     const Rule& at(int pos) const { return _rules.at(pos); }

     // lifecycle
     RuleSet();
     RuleSet(const RuleSet& rs);
     const RuleSet& operator=(const RuleSet& that);

     inline size_t slots() { return _rules.size(); }
     inline size_t length() { return _rules.size(); }
     /// Normalized Levenshtein distance
     /// cf. beijering et. al. p.2,5
     inline float nld() {
         return static_cast<float>(cost()) / slots();
     }
     int cost();
     int count(EditOp type);

     /// add a rule of the given type and figure out
     /// from, to, left + right context from the strings
     /// also modify the rule before with the correct
     /// right context
     void add_rule(EditOp type,
                   const string_impl& source, size_t spos,
                   const string_impl& target, size_t tpos);

 protected:
     /// just push a rule on the vector
     /// protected since it might confuse the rest of the
     /// rules
     void add_rule(const Rule& r);
     /// wrap two distinct merge processes to ensure the
     /// correct ordering
     inline void merge_rules() {
         merge_same_coordinate();
         merge_neighbors();
     }
     /// insert epsilon identity rules between each pair
     /// of substitution or identity rules
     void insert_epsilon_identity(const string_impl& source,
                                  const string_impl& target);

 private:
     /// merge rules that have the same coordinate - i.e.
     /// multiple deletions/insertions at the same position
     void merge_same_coordinate();
     /// merge neighboring non-identity rule - e.g. a substitution
     /// with a deletion or somesuch
     void merge_neighbors();
     /// helper function to delete a number of positions
     /// from the list, used by the merge functions
     void delete_positions(const std::list<int>& positions);
     /// helper function to determine the left context for rules
     char_impl left_context();

     /// the actual rule container
     std::vector<Rule> _rules;
};

/// hash structure so Rule can be used in an unordered map
struct RuleHasher {
    std::size_t operator()(const Rule& r) const;
};

}  // namespace Rulebased
}  // namespace Normalizer
}  // namespace Norma

/// ostream operator to ensure Rules can be outputted to cout or file
std::ostream& operator<<(std::ostream& strm,
                         const Norma::Normalizer::Rulebased::Rule& r);

/// ostream operator to ensure RuleSet can be outputted to cout or file
std::ostream& operator<<(std::ostream& strm,
                         const Norma::Normalizer::Rulebased::RuleSet& rs);
#endif  // NORMALIZER_RULEBASED_RULE_H_

