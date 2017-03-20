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
#define BOOST_TEST_MODULE Normalizer_External
#include<Python.h>
#include<map>
#include<string>
#include"tests/tests.h"
#include"config.h"
#include"normalizer/external.h"
#include"normalizer/result.h"

using Norma::Normalizer::External::External;
using Norma::Normalizer::Result;
using Norma::Normalizer::ResultSet;

const std::string TEST_PATH = std::string(TEST_BASE_DIR);

struct ExternalFixture {
    External *e;
    std::map<std::string, std::string> params;

    ExternalFixture() {
        e = new External();
        e->set_name("External");
        params["External.path"] = TEST_PATH;
        params["External.script"] = "normalize";
        e->set_from_params(params);
        e->init();
    }
    ~ExternalFixture() { delete e; }
};

BOOST_FIXTURE_TEST_SUITE(External1, ExternalFixture)

BOOST_AUTO_TEST_CASE(name_check) {
    const std::string name = "External";
    BOOST_CHECK_EQUAL(e->name(), name);
}

BOOST_AUTO_TEST_CASE(normalize_best) {
    Result r = (*e)("test");
    BOOST_CHECK_EQUAL(r.word, "foobar");
    BOOST_CHECK_EQUAL(r.score, 0.85);
}

BOOST_AUTO_TEST_CASE(normalize_nbest) {
    ResultSet expected = {Result("foo", 0.8), Result("bar", 0.2)};
    ResultSet rs = (*e)("test2", 2);
    BOOST_CHECK_EQUAL(rs.size(), 2);
    BOOST_CHECK(rs == expected);
}

BOOST_AUTO_TEST_SUITE_END()
