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
#define BOOST_TEST_MODULE Normalizer_Mapper
#include<map>
#include<string>
#include<tuple>
#include<boost/test/included/unit_test.hpp> // NOLINT[build/include_order]
#include"tests/tests.h"
#include"config.h"
#include"normalizer/exceptions.h"
#include"normalizer/mapper.h"
#include"normalizer/result.h"

using Norma::Normalizer::Mapper::Mapper;
using Norma::Normalizer::Result;
using Norma::Normalizer::ResultSet;

const std::string TEST_MAPFILE =
    std::string(TEST_BASE_DIR) + "/test-mapfile.txt";
const std::string TEST_MALFORMED_MAPFILE =
    std::string(TEST_BASE_DIR) + "/test-mapfile-malformed.txt";

struct MapperFixture {
    Mapper *m;

    MapperFixture() {
        m = new Mapper();
        m->set_name("Mapper");
        m->set_mapfile(TEST_MAPFILE);
        m->init();
    }
    ~MapperFixture() { delete m; }
};

BOOST_FIXTURE_TEST_SUITE(Mapper1, MapperFixture)

BOOST_AUTO_TEST_CASE(name_check) {
    const std::string name = "Mapper";
    BOOST_CHECK_EQUAL(m->name(), name);
}

BOOST_AUTO_TEST_CASE(normalize_best_vnd) {
    Result result = (*m)("vnd");
    auto message = std::get<2>(result.messages.front());
    BOOST_CHECK_EQUAL(result.word, "und");
    BOOST_CHECK_EQUAL(result.score, 1);
    BOOST_CHECK_EQUAL(message, "absolute count: 25");
}

BOOST_AUTO_TEST_CASE(normalize_best_vnnd) {
    Result result = (*m)("vnnd");
    auto message = std::get<2>(result.messages.front());
    BOOST_CHECK_EQUAL(result.word, "und");
    BOOST_CHECK_EQUAL(result.score, 1);
    BOOST_CHECK_EQUAL(message, "absolute count: 10");
}

BOOST_AUTO_TEST_CASE(normalize_best_jn) {
    Result result = (*m)("jn");
    auto message = std::get<2>(result.messages.front());
    BOOST_CHECK_EQUAL(result.word, "in");
    BOOST_CHECK_CLOSE(result.score, 0.75, 0.001);
    BOOST_CHECK_EQUAL(message, "absolute count: 75");
}

BOOST_AUTO_TEST_CASE(normalizer_n_best_1) {
    ResultSet given = (*m)("vnd", 3);
    ResultSet expected {Result("und", 1)};
    BOOST_CHECK(given == expected);
}

BOOST_AUTO_TEST_CASE(normalizer_n_best_2) {
    ResultSet given = (*m)("jn", 3);
    ResultSet expected {Result("in", 0.75),
                        Result("ihn", 0.2),
                        Result("inne", 0.05)};
    BOOST_REQUIRE_EQUAL(given.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        Result given_result = given[i],
            expected_result = expected[i];
        BOOST_CHECK_EQUAL(given_result.word, expected_result.word);
        BOOST_CHECK_CLOSE(given_result.score, expected_result.score, 0.001);
    }
}

BOOST_AUTO_TEST_CASE(normalizer_n_best_3) {
    ResultSet given = (*m)("jn", 2);
    ResultSet expected {Result("in", 0.75),
                        Result("ihn", 0.2)};
    BOOST_REQUIRE_EQUAL(given.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        Result given_result = given[i],
            expected_result = expected[i];
        BOOST_CHECK_EQUAL(given_result.word, expected_result.word);
        BOOST_CHECK_CLOSE(given_result.score, expected_result.score, 0.001);
    }
}

BOOST_AUTO_TEST_CASE(normalize_best_2) {
    Result result = (*m)("foo");
    auto message = std::get<2>(result.messages.front());
    BOOST_CHECK_EQUAL(result.word, "foo");
    BOOST_CHECK_EQUAL(result.score, 0);
    BOOST_CHECK_EQUAL(message, "word not found");
}

BOOST_AUTO_TEST_CASE(normalizer_clear) {
    Result before = (*m)("vnd");
    BOOST_CHECK_EQUAL(before.word, "und");
    BOOST_CHECK_EQUAL(before.score, 1);
    m->clear();
    Result after = (*m)("vnd");
    BOOST_CHECK_EQUAL(after.word, "vnd");
    BOOST_CHECK_EQUAL(after.score, 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Mapper2)

BOOST_AUTO_TEST_CASE(paramless_mapper) {
    Mapper m;
    m.init();
    Result foo = m("foo");
    BOOST_CHECK_EQUAL(foo.word, "foo");
    BOOST_CHECK_EQUAL(foo.score, 0);
}

BOOST_AUTO_TEST_CASE(non_existant_filename) {
    Mapper m;
    m.set_mapfile("<dummy>");
    BOOST_CHECK_THROW(m.init(), Norma::Normalizer::init_error);
}

BOOST_AUTO_TEST_CASE(malformed_paramfile) {
    Mapper m;
    m.set_mapfile(TEST_MALFORMED_MAPFILE);
    BOOST_CHECK_THROW(m.init(), Norma::Normalizer::init_error);
}

BOOST_AUTO_TEST_SUITE_END()
