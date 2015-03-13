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
#ifndef GFSM_CASCADE_H_
#define GFSM_CASCADE_H_
#include<mutex>
#include<set>
#include"gfsmlibs.h"
#include"semiring.h"
#include"labelvector.h"
#include"path.h"

namespace Gfsm {
class AutomatonBuilder;
class Automaton;

/// A cascade of finite-state automata.
/** Implements functions related to the cascade functionality of the
    Gfsmxl extension to the Gfsm library.
 */
class Cascade {
    friend class AutomatonBuilder;
 public:
    Cascade(const Cascade& a);
    Cascade(Cascade&& a);
    Cascade& operator=(Cascade a);
    ~Cascade() throw();

    /// Set the maximum number of paths returned during lookup.
    void set_max_paths(unsigned int n);
    /// Get the maximum number of paths returned during lookup.
    unsigned int get_max_paths() const;
    /// Set the maximum weight of paths for lookup.
    void set_max_weight(double w);
    /// Get the maximum weight of paths for lookup.
    double get_max_weight() const;
    /// Set the maximum number of operations allowed during lookup.
    void set_max_ops(unsigned int n);
    /// Get the maximum number of operations allowed during lookup.
    unsigned int get_max_ops() const;

    /// Append an automaton to the cascade.
    /** @param a The automaton to be appended.  Internally, a copy of
        the automaton is created, i.e., later changes to the automaton
        will have no effect on the cascade.
     */
    void append(const Automaton* a);
    /// Sort the cascade.
    /** Should be called after all automata have been added. */
    void sort();

    /// Finds the n-best paths for a given input sequence.
    /** @param v Input sequence for the cascade
        @return The set of Path objects accepted by this cascade
                with the lowest weights, depending on the values
                for get_max_paths(), get_max_weight(), and
                get_max_ops().
     */
    std::set<Path> lookup_nbest(const LabelVector& v) const;
    /// Finds the n-best paths for a given input sequence.
    /** @param v Input sequence for the cascade
        @param max_paths Maximum number of Paths to be returned;
                         this setting is stored for future lookups.
        @param max_weight Maximum allowed weight of the returned Paths;
                          this setting is stored for future lookups.
        @return The set of Path objects accepted by this cascade
                with the lowest weights, depending on get_max_ops().
     */
    std::set<Path> lookup_nbest(const LabelVector& v,
                                unsigned int max_paths,
                                double max_weight);

 protected:
    Cascade() = delete;
    Cascade(std::mutex* m, unsigned int depth = 2,
            SemiringType sr = SemiringType::TROPICAL);
    std::mutex* gfsm_mutex;
    gfsmxlCascade* _csc;
    gfsmxlCascadeLookup* _cl;
    unsigned int _size = 0;
};

}  // namespace Gfsm

#endif  // GFSM_CASCADE_H_

