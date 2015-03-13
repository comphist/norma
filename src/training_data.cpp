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
#include"training_data.h"
#include<stdexcept>
#include<vector>

namespace Norma {
////////////////////////////////// TrainingPair ///////////////////////////////

const string_impl& TrainingPair::source() const {
    return _source;
}

const string_impl& TrainingPair::target() const {
    return _target;
}

bool TrainingPair::is_used() const {
    return _used;
}

void TrainingPair::make_used() {
    _used = true;
    if (_data == nullptr)
        return;
    _data->_used[_pos] = true;
}

TrainingPair::TrainingPair(TrainingData* data, size_t pos) {
    _data = data;
    _pos = pos;
    _used = _data->_used[pos];
    _source = _data->_source[pos];
    _target = _data->_target[pos];
}

////////////////////////////////// TrainingData ///////////////////////////////
TrainingData& TrainingData::add_pair(const string_impl& source,
                                     const string_impl& target) {
    return add_source(source).add_target(target);
}

TrainingData& TrainingData::add_source(const string_impl& source) {
    return add_token(source, &_source);
}

TrainingData& TrainingData::add_target(const string_impl& target) {
    return add_token(target, &_target);
}

TrainingData& TrainingData::add_token(const string_impl& token,
                                      std::vector<string_impl>* where) {
    where->push_back(token);
    if (_target.size() == _source.size())
        _used.push_back(false);
    return *this;
}

size_t TrainingData::length() const {
    size_t slen = _source.size(),
           tlen = _target.size();
    return (slen < tlen) ? slen : tlen;
}

bool TrainingData::empty() const {
    return length() == 0;
}

TrainingData::iterator TrainingData::begin() {
    return iterator(this, 0);
}

TrainingData::iterator TrainingData::end() {
    return iterator(this, length());
}

TrainingData::reverse_iterator TrainingData::rbegin() {
    return reverse_iterator(this, 0);
}

TrainingData::reverse_iterator TrainingData::rend() {
    return reverse_iterator(this, length());
}

TrainingPair TrainingData::get_pair(size_t pos, IteratorDirection direction) {
    // this is a bit hacky:
    // pos gets inverted on length for reverse iterators
    // if pos == length() (for rend), it will become the largest
    // possible value (implementation dependent) since size_t is
    // unsigned. if it gets incremented after that, it will be 0 again
    // so it points to the first element.
    if (direction == IteratorDirection::REV)
        pos = length() - (pos + 1);
    if (pos >= length())
        throw std::out_of_range("TrainingData out of range");
    return TrainingPair(this, pos);
}
}  // namespace Norma
