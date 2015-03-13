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
#ifndef TRAINING_DATA_INL_H_
#define TRAINING_DATA_INL_H_
namespace Norma {
template<IteratorDirection D>
TrainingData::basic_iterator<D>::basic_iterator(TrainingData* data,
                                                size_t pos)
    : _data(data), _pos(pos) {}

template<IteratorDirection D>
TrainingPair& TrainingData::basic_iterator<D>::operator*() {
    // the pair needs to be stored as member since
    // operator->() needs to return a ptr. it shouldn't
    // be a big deal anyway since TrainingPair is a fairly
    // light weight class.
    if (!has_pair)
        _pair = _data->get_pair(_pos, D);
    return _pair;
}

template<IteratorDirection D>
TrainingPair* TrainingData::basic_iterator<D>::operator->() {
    return &(operator*());
}

template<IteratorDirection D>
TrainingData::basic_iterator<D>& TrainingData::basic_iterator<D>::operator++() {
    ++_pos;
    return *this;
}

template<IteratorDirection D>
TrainingData::basic_iterator<D>& TrainingData::basic_iterator<D>::operator--() {
    --_pos;
    return *this;
}

template<IteratorDirection D>
bool TrainingData::basic_iterator<D>::operator==
    (const TrainingData::basic_iterator<D>& that) {
    return this->_data == that._data
        && this->_pos == that._pos;
}

template<IteratorDirection D>
bool TrainingData::basic_iterator<D>::operator!=
    (const TrainingData::basic_iterator<D>& that) {
    return !(*this == that);
}
}  // namespace Norma
#endif  // TRAINING_DATA_INL_H_

