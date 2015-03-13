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
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TrainingData
#include<vector>
#include<string>
#include<boost/test/included/unit_test.hpp>  // NOLINT[build/include_order]
#include"training_data.h"
#include"string_impl.h"
#include"tests.h"

struct TrainingDataFixture {
    std::vector<std::string> strings { "foo", "bar", "baz", "bla" };
    Norma::TrainingData data;
};

BOOST_FIXTURE_TEST_SUITE(TrainingData1, TrainingDataFixture)

BOOST_AUTO_TEST_CASE(training_data) {
    data.add_source(strings[0].c_str());
    BOOST_CHECK_EQUAL(data.length(), 0);
    BOOST_CHECK(data.empty());
    data.add_target(strings[1].c_str());
    BOOST_CHECK_EQUAL(data.length(), 1);
    data.add_target(strings[2].c_str());
    data.add_source(strings[3].c_str());
    BOOST_CHECK_EQUAL(data.length(), 2);
    Norma::TrainingData::iterator pp = data.begin();
    BOOST_CHECK_EQUAL(pp->source(), strings[0].c_str());
    BOOST_CHECK_EQUAL(pp->target(), strings[1].c_str());
    ++pp;
    BOOST_CHECK_EQUAL(pp->source(), strings[3].c_str());
    BOOST_CHECK_EQUAL(pp->target(), strings[2].c_str());
    --pp;
    BOOST_CHECK_EQUAL(pp->source(), strings[0].c_str());
    BOOST_CHECK_EQUAL(pp->target(), strings[1].c_str());
    ++pp; ++pp;
    BOOST_CHECK_THROW(*pp, std::out_of_range);
    Norma::TrainingData::reverse_iterator pq = data.rbegin();
    BOOST_CHECK_EQUAL(pq->source(), strings[3].c_str());
    BOOST_CHECK_EQUAL(pq->target(), strings[2].c_str());
    ++pq;
    BOOST_CHECK_EQUAL(pq->source(), strings[0].c_str());
    BOOST_CHECK_EQUAL(pq->target(), strings[1].c_str());
    --pq;
    BOOST_CHECK_EQUAL(pq->source(), strings[3].c_str());
    BOOST_CHECK_EQUAL(pq->target(), strings[2].c_str());
    ++pq; ++pq;
    BOOST_CHECK_THROW(*pq, std::out_of_range);
}

BOOST_AUTO_TEST_SUITE_END()
