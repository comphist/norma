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
#ifndef GFSM_LABELVECTOR_H_
#define GFSM_LABELVECTOR_H_
#include<initializer_list>
#include"gfsmlibs.h"

namespace Gfsm {

const gfsmLabelVal EPSILON_LABEL = 0;

class Acceptor;

/// A vector of label values.
class LabelVector {
    friend class Acceptor;
    friend class Transducer;
    friend class Cascade;
 public:
    /// Construct an empty label vector.
    LabelVector() : _vec(g_ptr_array_new()) {}
    /// Construct a label vector from a list of numeric labels.
    explicit LabelVector(std::initializer_list<gfsmLabelVal> args);
    LabelVector(const LabelVector& v);
    LabelVector(LabelVector&& v);
    LabelVector& operator=(LabelVector v);
    ~LabelVector();

    /// Get an element from the vector.
    gfsmLabelVal get(unsigned int n) const;
    /// Append an element to the end of the vector.
    void push_back(const gfsmLabelVal value);
    /// Clear the vector.
    void clear();
    /// Get the size of the vector.
    unsigned int size() const {return _vec->len;}

    bool operator==(const LabelVector& that) const;
    inline bool operator!=(const LabelVector& that) const {
        return !(*this == that);
    }
    bool operator<(const LabelVector& that) const;

    /// Iterator class for LabelVector.
    class const_iterator {
     friend class LabelVector;
     public:
         gfsmLabelVal operator*() const;
         const_iterator& operator++();
         bool operator==(const const_iterator& that) const;
         bool operator!=(const const_iterator& that) const;
     protected:
         const_iterator(unsigned int pos, const LabelVector * const vec);
     private:
         unsigned int _pos;
         const LabelVector* _vec;
    };
    const_iterator begin() const;
    const_iterator end() const;

 private:
    gfsmLabelVector *_vec;
};

}  // namespace Gfsm

#endif  // GFSM_LABELVECTOR_H_
