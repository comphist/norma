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
#include"labelvector.h"
#include<stdexcept>
#include<utility>
#include<algorithm>
#include<glib.h>  // NOLINT[build/include_order]

namespace Gfsm {

LabelVector::LabelVector(std::initializer_list<gfsmLabelVal> args) :
    _vec(g_ptr_array_sized_new(args.size())) {
    for (auto iter = args.begin(); iter != args.end(); ++iter) {
        push_back(*iter);
    }
}

LabelVector::LabelVector(const LabelVector& v) :
    _vec(g_ptr_array_sized_new(v.size())) {
    for (size_t i = 0; i < v.size(); ++i) {
        g_ptr_array_add(_vec, GUINT_TO_POINTER(v.get(i)));
    }
}

LabelVector::LabelVector(LabelVector&& v) : LabelVector() {
    std::swap(_vec, v._vec);
}

LabelVector& LabelVector::operator=(LabelVector v) {
    std::swap(_vec, v._vec);
    return *this;
}

LabelVector::~LabelVector() {
    g_ptr_array_free(_vec, 1);
}

bool LabelVector::operator==(const LabelVector& that) const {
    size_t i = 0, j = 0;
    while (i < this->_vec->len && j < that._vec->len) {
        while (this->get(i) == EPSILON_LABEL)
            ++i;
        while (that.get(j) == EPSILON_LABEL)
            ++j;
        if (this->get(i++) != that.get(j++))
            return false;
    }
    return true;
}

bool LabelVector::operator<(const LabelVector& that) const {
    if (_vec->len != that._vec->len) {
        return (_vec->len < that._vec->len);
    }
    for (unsigned int i = 0; i < _vec->len; ++i) {
        gfsmLabelVal x = get(i);
        gfsmLabelVal y = that.get(i);
        if (x != y)
            return (x < y);
    }
    return false;
}

gfsmLabelVal LabelVector::get(unsigned int n) const {
    if (n >= _vec->len) {
        throw std::out_of_range("LabelVector: out of bounds");
    }
    return (gfsmLabelVal) GPOINTER_TO_UINT(g_ptr_array_index(_vec, n));
}

void LabelVector::push_back(const gfsmLabelVal value) {
    g_ptr_array_add(_vec, GUINT_TO_POINTER(value));
}

void LabelVector::clear() {
    g_ptr_array_set_size(_vec, 0);
}

LabelVector::const_iterator LabelVector::begin() const {
    return LabelVector::const_iterator(0, this);
}

LabelVector::const_iterator LabelVector::end() const {
    return LabelVector::const_iterator(_vec->len, this);
}


LabelVector::const_iterator::const_iterator(unsigned int pos,
                                            const LabelVector * const vec) {
    _pos = pos;
    _vec = vec;
}

gfsmLabelVal LabelVector::const_iterator::operator*() const {
    return _vec->get(_pos);
}

LabelVector::const_iterator& LabelVector::const_iterator::operator++() {
    ++_pos;
    if (_pos > _vec->size())
        throw std::out_of_range("LabelVector position out of range");
    return *this;
}

bool LabelVector::const_iterator::operator==
        (const LabelVector::const_iterator& that) const {
    return (this->_vec == that._vec && this->_pos == that._pos);
}

bool LabelVector::const_iterator::operator!=
        (const LabelVector::const_iterator& that) const {
    return !(*this == that);
}


}  // namespace Gfsm
