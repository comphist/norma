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
#define BOOST_TEST_MODULE Normalizer_Lexicon
#include<algorithm>
#include<map>
#include<stdexcept>
#include<string>
#include<tuple>
#include<iostream>
#include<vector>
#include<boost/test/included/unit_test.hpp>  // NOLINT[build/include_order]
#include"gfsmlibs.h"
#include"config.h"
#include"mock_lexicon.h"
#include"normalizer/exceptions.h"
#include"lexicon/lexicon.h"

using Norma::Normalizer::Lexicon;

const std::string TEST_FSMFILE =
    std::string(TEST_BASE_DIR) + "/test-lexicon.gfsa";
const std::string TEST_LABFILE =
    std::string(TEST_BASE_DIR) + "/test-lexicon.lab";

BOOST_AUTO_TEST_SUITE(Lexicon1)

BOOST_AUTO_TEST_CASE(lexicon_absolute_filenames) {
    const std::string fsmfile = TEST_FSMFILE;
    const std::string labfile = TEST_LABFILE;

    std::map<std::string, std::string> params;
    params["Lexicon.fsmfile"] = fsmfile;
    params["Lexicon.symfile"] = labfile;

    Lexicon lex;
    lex.set_from_params(params);
    BOOST_CHECK_EQUAL(lex.get_lexfile(), fsmfile);
    BOOST_CHECK_EQUAL(lex.get_symfile(), labfile);
}

BOOST_AUTO_TEST_CASE(lexicon_relative_filenames) {
    const std::string parent  = std::string(TEST_BASE_DIR);
    const std::string fsmfile_full = parent + "/test-lexicon.gfsa";
    const std::string labfile_full = parent + "/test-lexicon.lab";

    std::map<std::string, std::string> params;
    params["Lexicon.fsmfile"] = "test-lexicon.gfsa";
    params["Lexicon.symfile"] = "test-lexicon.lab";
    params["parent_path"] = parent;

    Lexicon lex;
    lex.set_from_params(params);
    BOOST_CHECK_EQUAL(lex.get_lexfile(), fsmfile_full);
    BOOST_CHECK_EQUAL(lex.get_symfile(), labfile_full);
}

BOOST_AUTO_TEST_SUITE_END()

struct LexiconFixture {
    Lexicon lex;

    LexiconFixture() {
        lex.set_lexfile(TEST_FSMFILE);
        lex.set_symfile(TEST_LABFILE);
        lex.init();
    }
    ~LexiconFixture() { }
};

BOOST_FIXTURE_TEST_SUITE(Lexicon2, LexiconFixture)

BOOST_AUTO_TEST_CASE(lexicon_contains_partial) {
    BOOST_CHECK(lex.contains_partial(""));
    BOOST_CHECK(lex.contains_partial("z"));
    BOOST_CHECK(lex.contains_partial("zw"));
    BOOST_CHECK(!lex.contains_partial("zwa"));
    BOOST_CHECK(lex.contains_partial("zwe"));
    BOOST_CHECK(lex.contains_partial("zwei"));
    BOOST_CHECK(lex.contains_partial("zweit"));
    BOOST_CHECK(!lex.contains_partial("zweig"));
}

BOOST_AUTO_TEST_CASE(lexicon_contains) {
    BOOST_CHECK(lex.contains("eins"));
    BOOST_CHECK(lex.contains("zwei"));
    BOOST_CHECK(!lex.contains("zweite"));
    BOOST_CHECK(lex.contains("zweitens"));
}

BOOST_AUTO_TEST_CASE(lexicon_add_word) {
    BOOST_REQUIRE(!lex.contains("zweite"));
    lex.add("zweite");
    BOOST_CHECK(lex.contains("zweite"));
    // try again with string containing an undefined symbol ("ß")
    BOOST_REQUIRE(!lex.contains("naß"));
    lex.add("naß");
    BOOST_CHECK(lex.contains("naß"));
    BOOST_CHECK(lex.contains_partial("n"));
    BOOST_CHECK(lex.contains_partial("na"));
    BOOST_CHECK(lex.contains_partial("naß"));
}

BOOST_AUTO_TEST_CASE(lexicon_entries) {
    std::vector<string_impl> ls = lex.entries();
    BOOST_CHECK(std::count(ls.begin(), ls.end(), "eins") > 0);
    BOOST_CHECK(std::count(ls.begin(), ls.end(), "zwei") > 0);
    BOOST_CHECK(std::count(ls.begin(), ls.end(), "zweite") == 0);
    BOOST_CHECK(std::count(ls.begin(), ls.end(), "zweitens") > 0);
}

BOOST_AUTO_TEST_CASE(lexicon_entries_2) {
    std::vector<string_impl> lx = lex.entries();
    BOOST_REQUIRE(std::count(lx.begin(), lx.end(), "zweite") == 0);
    lex.add("zweite");
    std::vector<string_impl> ly = lex.entries();
    BOOST_REQUIRE(std::count(ly.begin(), ly.end(), "zweite") > 0);
}

BOOST_AUTO_TEST_CASE(lexicon_size) {
    BOOST_CHECK_EQUAL(lex.size(), 12);
    std::vector<string_impl> entries = lex.entries();
    BOOST_CHECK_EQUAL(entries.size(), 12);
}

BOOST_AUTO_TEST_SUITE_END()

struct Lexicon3Fixture {
    Lexicon lex;

    Lexicon3Fixture() {
        lex.init();
    }
    ~Lexicon3Fixture() { }
};

BOOST_FIXTURE_TEST_SUITE(Lexicon3, Lexicon3Fixture)

BOOST_AUTO_TEST_CASE(lexicon_add_from_scratch) {
    BOOST_CHECK(!lex.contains("an"));
    BOOST_CHECK(!lex.contains_partial("an"));
    lex.add("an");
    BOOST_CHECK(lex.contains("an"));
    BOOST_CHECK(lex.contains_partial("a"));
    BOOST_CHECK(lex.contains_partial("an"));
    BOOST_CHECK(!lex.contains("a"));
}

BOOST_AUTO_TEST_CASE(non_existant_filename) {
    lex.set_lexfile("<dummy>");
    lex.set_symfile("<dummy>");
    BOOST_CHECK_THROW(lex.init(), Norma::Normalizer::init_error);
}

BOOST_AUTO_TEST_CASE(incomplete_paramfiles_1) {
    lex.set_lexfile(TEST_FSMFILE);
    BOOST_CHECK_THROW(lex.init(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(incomplete_paramfiles_2) {
    lex.set_symfile(TEST_LABFILE);
    BOOST_CHECK_THROW(lex.init(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(malformed_paramfile_1) {
    lex.set_lexfile(TEST_LABFILE);  // whoops, made a mistake here!
    lex.set_symfile(TEST_LABFILE);
    BOOST_CHECK_THROW(lex.init(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(malformed_paramfile_2) {
    lex.set_lexfile(TEST_FSMFILE);
    lex.set_symfile(TEST_FSMFILE);  // and the other way around
    BOOST_CHECK_THROW(lex.init(), std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()

struct MockLexiconFixture {
    MockLexicon lex;

    MockLexiconFixture() {}
    ~MockLexiconFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(MockLexicon1, MockLexiconFixture)

BOOST_AUTO_TEST_CASE(mock_lexicon_contains_partial) {
    BOOST_CHECK(lex.contains_partial(""));
    BOOST_CHECK(lex.contains_partial("e"));
    BOOST_CHECK(lex.contains_partial("ei"));
    BOOST_CHECK(lex.contains_partial("ein"));
    BOOST_CHECK(lex.contains_partial("eins"));
    BOOST_CHECK(!lex.contains_partial("einse"));
    BOOST_CHECK(lex.contains_partial("z"));
    BOOST_CHECK(lex.contains_partial("dre"));
    BOOST_CHECK(lex.contains_partial("un"));
    BOOST_CHECK(!lex.contains_partial("a"));
}

BOOST_AUTO_TEST_CASE(mock_lexicon_contains) {
    BOOST_CHECK(lex.contains("eins"));
    BOOST_CHECK(lex.contains("zwei"));
    BOOST_CHECK(lex.contains("drei"));
    BOOST_CHECK(lex.contains("und"));
    BOOST_CHECK(!lex.contains("vier"));
    BOOST_CHECK(!lex.contains("e"));
    BOOST_CHECK(!lex.contains("ei"));
    BOOST_CHECK(!lex.contains("ein"));
}

BOOST_AUTO_TEST_SUITE_END()
