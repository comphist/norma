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
// TODO(fpetran): TrainingData and TrainingPair could really benefit
// from being changed to pimpl to eliminate the dependency on vector
// and prevent recompiles when changing the internal representation
// of the data.
#ifndef TRAINING_DATA_H_
#define TRAINING_DATA_H_
#include<vector>
#include"string_impl.h"

namespace Norma {
class TrainingData;

enum class IteratorDirection { FWD, REV };

/** Represent a TrainingPair of source, target words
 *  and also store if the pair has been used in training
 *  already.
 **/
class TrainingPair {
    friend class TrainingData;
 public:
    TrainingPair() = default;
    const string_impl& source() const;
    const string_impl& target() const;
    bool is_used() const;
    /// mark this pair as used. note that you cannot make it unused again!
    void make_used();

 protected:
    /// used by the iterator to construct a
    /// a certain point in the data.
    /// classes can't be forward declared,
    /// TrainingData as friend, which will also
    /// of the construction.
    TrainingPair(TrainingData* data, size_t pos);

 private:
    string_impl _source = "", _target = "";
    bool _used = false;
    TrainingData* _data = nullptr;
    size_t _pos;
};

/** Representation of training data for all Normalizer classes.
 *  source and target can be added independent from each other. TrainingData
 *  also stores whether the pairs have already been used for training. Note
 *  that that storage is independent of the Normalizer, so those should not
 *  set used by themselves but instead rely on another class to set it for them.
 **/
class TrainingData {
    friend class TrainingPair;
 public:
    /// add a source token for a training pair
    /// the user needs to keep tabs on the pairs that have been added.
    TrainingData& add_source(const string_impl& source);
    /// add a target token for a training pair
    /// the user needs to keep tabs on the pairs that have been added.
    TrainingData& add_target(const string_impl& target);
    /// convenience function to add a complete pair.
    TrainingData& add_pair(const string_impl& source,
                           const string_impl& target);
    /// return the amount of training pairs - unassigned source or target
    /// are ignored.
    size_t length() const;
    bool empty() const;
    /** Template class for forward and reverse bidirectional iterators.
     *  it does not currently have postfix increment/decrement or comparison
     *  operators, but those would be trivial to implement.
     **/
    template<IteratorDirection D> class basic_iterator {
        friend class TrainingData;
     public:
        TrainingPair& operator*();
        TrainingPair* operator->();
        basic_iterator<D>& operator++();
        basic_iterator<D>& operator--();
        bool operator==(const basic_iterator<D>& that);
        bool operator!=(const basic_iterator<D>& that);

     protected:
        basic_iterator<D>(TrainingData* data, size_t pos);

     private:
        TrainingData* _data = nullptr;
        size_t _pos;
        bool has_pair = false;
        TrainingPair _pair;
    };
    template<IteratorDirection D> friend class basic_iterator;
    /// bidirectional forward iterator over TrainingData
    typedef basic_iterator<IteratorDirection::FWD> iterator;
    /// bidirectional reverse iterator over TrainingData
    typedef basic_iterator<IteratorDirection::REV> reverse_iterator;
    iterator begin();
    iterator end();
    reverse_iterator rbegin();
    reverse_iterator rend();

 protected:
    /// this method is used by the iterator to construct a pair for
    /// dereferencing.
    TrainingPair get_pair(size_t pos, IteratorDirection direction);
    std::vector<string_impl> _source, _target;
    std::vector<bool> _used;

 private:
    TrainingData& add_token(const string_impl& token,
                            std::vector<string_impl>* where);
};
}  // namespace Norma

// definitions of template class members
#include"training_data-inl.h"
#endif  // TRAINING_DATA_H_

