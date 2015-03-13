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
#ifndef TESTS_TESTS_H_
#define TESTS_TESTS_H_
#include<boost/test/included/unit_test.hpp>
#include"string_impl.h"
#include"normalizer/rulebased/rule.h"

// Make BOOST_x_EQUAL work with string_impl
// --taken from <http://stackoverflow.com/questions/17572583/boost-check-fails-to-compile-operator-for-custom-types>

namespace boost {
namespace test_tools {
#ifdef USE_ICU_STRING
    template<>
    struct print_log_value<string_impl> {
        void operator()(std::ostream& os, string_impl const& si) {  // NOLINT[runtime/references]
            ::operator<<(os, si);
        }
    };
#endif  // USE_ICU_STRING

    template<>
    struct print_log_value<Norma::Normalizer::Rulebased::Rule> {
        void operator()(std::ostream& os, Norma::Normalizer::Rulebased::Rule const& r) {  // NOLINT[runtime/references]
            ::operator<<(os, r);
        }
    };
}  // namespace test_tools
}  // namespace boost

#endif  // TESTS_TESTS_H_
