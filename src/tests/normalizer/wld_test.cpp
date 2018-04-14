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
#define BOOST_TEST_MODULE Normalizer_WLD
#include<algorithm>
#include<initializer_list>
#include<map>
#include<set>
#include<string>
#include<vector>
#include"tests/tests.h"
#include"config.h"
#include"normalizer/exceptions.h"
#include"lexicon/lexicon.h"
#include"normalizer/result.h"
#include"normalizer/wld.h"
#include"normalizer/wld/levenshtein_algorithm.h"
#include"normalizer/wld/levenshtein_aligner.h"
#include"gfsm_wrapper.h"
#include"string_impl.h"
#include"training_data.h"

using namespace Norma::Normalizer::WLD;  // NOLINT[build/namespaces]
using Norma::Normalizer::Lexicon;
using Norma::Normalizer::Result;
using Norma::Normalizer::ResultSet;
using Gfsm::StringPath;

const std::string TEST_WEIGHTSFILE =
    std::string(TEST_BASE_DIR) + "/test-weights.txt";
const std::string TEST_MALFORMED_WEIGHTSFILE =
    std::string(TEST_BASE_DIR) + "/test-weights-malformed.txt";

//////// WeightSet /////////////////////////////////////////////////////////////

struct WeightSetFixture {
    WeightSet ws;
};

BOOST_FIXTURE_TEST_SUITE(WeightSet1, WeightSetFixture)

BOOST_AUTO_TEST_CASE(ws_default_costs) {
    BOOST_CHECK_CLOSE(ws.default_identity_cost(), 0.0, 0.00001);
    BOOST_CHECK_CLOSE(ws.default_replacement_cost(), 1.0, 0.00001);
    BOOST_CHECK_CLOSE(ws.default_insertion_cost(), 1.0, 0.00001);
    BOOST_CHECK_CLOSE(ws.default_deletion_cost(), 1.0, 0.00001);
    ws.default_identity_cost() = 0.25;
    ws.default_replacement_cost() = 0.5;
    ws.default_insertion_cost() = 2.0;
    ws.default_deletion_cost() = 0.75;
    BOOST_CHECK_CLOSE(ws.default_identity_cost(), 0.25, 0.00001);
    BOOST_CHECK_CLOSE(ws.default_replacement_cost(), 0.5, 0.00001);
    BOOST_CHECK_CLOSE(ws.default_insertion_cost(), 2.0, 0.00001);
    BOOST_CHECK_CLOSE(ws.default_deletion_cost(), 0.75, 0.00001);
}

BOOST_AUTO_TEST_CASE(ws_add_weight) {
    BOOST_CHECK_EQUAL(ws.size(), 0);
    ws.add_weight("v", "u", 0.5);
    BOOST_REQUIRE_EQUAL(ws.size(), 1);
    StringPath path = ws.weights().at(0);
    BOOST_CHECK(path.get_input()  == std::vector<string_impl>{"v"});
    BOOST_CHECK(path.get_output() == std::vector<string_impl>{"u"});
    BOOST_CHECK_CLOSE(path.get_weight(), 0.5, 0.00001);
    ws.add_weight("v", "u", 0.2);
    BOOST_REQUIRE_EQUAL(ws.size(), 1);
}

BOOST_AUTO_TEST_CASE(ws_get_weight) {
    // set up some stuff
    ws.add_weight("v", "u", 0.5);
    ws.add_weight("v", Symbols::EPS, 0.25);
    ws.add_weight("jn", "xy", 0.75);
    ws.default_identity_cost() = 0.33;
    ws.default_replacement_cost() = 0.66;
    ws.default_insertion_cost() = 0.88;
    ws.default_deletion_cost() = 0.44;
    BOOST_REQUIRE_EQUAL(ws.size(), 3);
    // check
    BOOST_CHECK_CLOSE(ws.get_weight("v", "u"), 0.5, 0.0001);
    BOOST_CHECK_CLOSE(ws.get_weight("v", Symbols::EPS), 0.25, 0.0001);
    BOOST_CHECK_CLOSE(ws.get_weight("jn", "xy"), 0.75, 0.0001);
    BOOST_CHECK_CLOSE(ws.get_weight("v", "v"),
                      ws.default_identity_cost(), 0.0001);
    BOOST_CHECK_CLOSE(ws.get_weight("v", "xy"),
                      (ws.default_replacement_cost()
                       + ws.default_insertion_cost()), 0.0001);
    BOOST_CHECK_CLOSE(ws.get_weight(Symbols::EPS, "u"),
                      ws.default_insertion_cost(), 0.0001);
    BOOST_CHECK_CLOSE(ws.get_weight("jn", Symbols::EPS),
                      (ws.default_deletion_cost() * 2), 0.0001);
}

BOOST_AUTO_TEST_CASE(ws_add_weight_with_empty_string) {
    ws.add_weight("jn", "", 0.4);
    BOOST_REQUIRE_EQUAL(ws.size(), 1);
    const StringPath path = ws.weights().at(0);
    const std::vector<string_impl> jn {"j", "n"};
    BOOST_CHECK(path.get_input()  == jn);
    BOOST_CHECK(path.get_output() == std::vector<string_impl>());
    BOOST_CHECK_CLOSE(path.get_weight(), 0.4, 0.00001);
}

BOOST_AUTO_TEST_CASE(ws_add_weight_with_epsilon) {
    ws.add_weight("jn", Symbols::EPS, 0.4);
    BOOST_REQUIRE_EQUAL(ws.size(), 1);
    const StringPath path = ws.weights().at(0);
    const std::vector<string_impl> jn {"j", "n"};
    BOOST_CHECK(path.get_input()  == jn);
    BOOST_CHECK(path.get_output() == std::vector<string_impl>());
    BOOST_CHECK_CLOSE(path.get_weight(), 0.4, 0.00001);
}

BOOST_AUTO_TEST_CASE(ws_input_symbols) {
    ws.add_weight("v",  "u", 0.5);
    ws.add_weight("nn", "n", 0.5);
    ws.add_weight("jn", "n", 0.5);
    ws.add_weight("ä", "ae", 0.5);
    string_impl input_symbols("äjnv");
    std::set<string_impl> expected;
    // 'expected' may be {"ä", "j", "n", "v"} or something else,
    // depending on how string_impl treats the umlaut ...
    for (string_size i = 0; i < input_symbols.length(); ++i) {
        expected.insert(from_char(input_symbols[i]));
    }
    const auto& symbols = ws.input_symbols();
    BOOST_REQUIRE_EQUAL(symbols.size(), expected.size());
    BOOST_CHECK(symbols == expected);
}

BOOST_AUTO_TEST_CASE(ws_read_paramfile) {
    ws.read_paramfile(TEST_WEIGHTSFILE);
    BOOST_REQUIRE_EQUAL(ws.size(), 6);
    const std::map<EditPair, double>& weights = ws.weight_map();
    const std::map<EditPair, double> expected {
        std::make_pair(EditPair({"j"}, {"i"}), 0.2),
        std::make_pair(EditPair({"j"}, {"i", "h"}), 0.8),
        std::make_pair(EditPair({"n"}, {"m"}), 0.9),
        std::make_pair(EditPair({"n", "n"}, {"n"}), 0.5),
        std::make_pair(EditPair({"x"}, {}), 0.99),
        std::make_pair(EditPair({"q"}, {}), 0.1)};
    for (const auto& elem : weights) {
        BOOST_REQUIRE(expected.count(elem.first) > 0);
        BOOST_CHECK_CLOSE(elem.second, expected.at(elem.first), 0.0001);
    }
}

BOOST_AUTO_TEST_SUITE_END()

//////// LevenshteinAlgorithm //////////////////////////////////////////////////

struct LevenshteinAlgorithmFixture {
    WeightSet ws;

    LevenshteinAlgorithmFixture() {
        ws.read_paramfile(TEST_WEIGHTSFILE);
    }
};

BOOST_FIXTURE_TEST_SUITE(LevenshteinAlgorithm1, LevenshteinAlgorithmFixture)

BOOST_AUTO_TEST_CASE(la_wld1) {
    BOOST_CHECK_CLOSE(wld("j", "i", ws),  0.2, 0.0001);
    BOOST_CHECK_CLOSE(wld("n", "m", ws),  0.9, 0.0001);
    BOOST_CHECK_CLOSE(wld("x", "", ws),   0.99, 0.0001);
    BOOST_CHECK_CLOSE(wld("x", "y", ws),  1.0, 0.0001);
    BOOST_CHECK_CLOSE(wld("x", "x", ws),  0.0, 0.0001);
    BOOST_CHECK_CLOSE(wld("", "y", ws),   1.0, 0.0001);
    BOOST_CHECK_CLOSE(wld("y", "", ws),   1.0, 0.0001);
    // WLD calculation does not consider multi-character weights a.t.m.,
    // so the values below differ from what is in the test-weights.txt file
    BOOST_CHECK_CLOSE(wld("nn", "n", ws), 1.0, 0.0001);
    BOOST_CHECK_CLOSE(wld("j", "ih", ws), 1.2, 0.0001);
}

BOOST_AUTO_TEST_CASE(la_wld2) {
    BOOST_CHECK_CLOSE(wld("jnx", "im", ws), 2.09, 0.0001);
    BOOST_CHECK_CLOSE(wld("jn", "ein", ws), 1.2, 0.0001);
    BOOST_CHECK_CLOSE(wld("exxxn", "n", ws), 3.97, 0.0001);
    BOOST_CHECK_CLOSE(wld("jq", "ni", ws), 1.3, 0.0001);
}

BOOST_AUTO_TEST_CASE(la_align1) {
    AlignmentSet set = align("j", "i", ws);
    BOOST_REQUIRE_EQUAL(set.size(), 1);
    RuleSet rs = set.at(0);
    BOOST_REQUIRE_EQUAL(rs.size(), 1);
    EditPair pair = rs.at(0);
    BOOST_CHECK(pair.first  == std::vector<string_impl>({"j"}));
    BOOST_CHECK(pair.second == std::vector<string_impl>({"i"}));
}

BOOST_AUTO_TEST_CASE(la_align2) {
    AlignmentSet set = align("jq", "ni", ws);
    BOOST_REQUIRE_EQUAL(set.size(), 1);
    RuleSet rs = set.at(0);
    BOOST_REQUIRE_EQUAL(rs.size(), 3);
    EditPair pair = rs.at(0);
    BOOST_CHECK(pair.first.empty());
    BOOST_CHECK(pair.second == std::vector<string_impl>({"n"}));
    pair = rs.at(1);
    BOOST_CHECK(pair.first  == std::vector<string_impl>({"j"}));
    BOOST_CHECK(pair.second == std::vector<string_impl>({"i"}));
    pair = rs.at(2);
    BOOST_CHECK(pair.first  == std::vector<string_impl>({"q"}));
    BOOST_CHECK(pair.second.empty());
}

BOOST_AUTO_TEST_CASE(la_align3) {
    AlignmentSet set = align("a", "bc", ws);
    BOOST_REQUIRE_EQUAL(set.size(), 2);
    RuleSet expected_a, expected_b;
    expected_a.push_back(EditPair({"a"}, {"b"}));
    expected_a.push_back(EditPair({}, {"c"}));
    expected_b.push_back(EditPair({}, {"b"}));
    expected_b.push_back(EditPair({"a"}, {"c"}));
    RuleSet result_a = set.at(0),
            result_b = set.at(1);
    BOOST_CHECK((result_a == expected_a && result_b == expected_b)
                || (result_a == expected_b && result_b == expected_a));
}

BOOST_AUTO_TEST_SUITE_END()

//////// LevenshteinAligner ////////////////////////////////////////////////////

struct LevenshteinAlignerFixture {
    LevenshteinAligner* aligner;
    WeightSet ws;

    LevenshteinAlignerFixture() {
        aligner = new LevenshteinAligner(ws, 3, 7);
    }
    ~LevenshteinAlignerFixture() {
        delete aligner;
    }
};

BOOST_FIXTURE_TEST_SUITE(LevenshteinAligner1, LevenshteinAlignerFixture)

BOOST_AUTO_TEST_CASE(aligner_check_defaults) {
    BOOST_CHECK_CLOSE(aligner->learning_rate(), 0.2, 0.001);
    BOOST_CHECK_EQUAL(aligner->meandiff(), 0);
    BOOST_CHECK(!aligner->allow_pure_insertions());
    BOOST_CHECK(!aligner->allow_identity());
    BOOST_CHECK(aligner->weight_set().empty());
}

BOOST_AUTO_TEST_SUITE_END()

//////// WLD normalizer ////////////////////////////////////////////////////////

struct WLDFixture {
    WLD* w;
    Lexicon* lex;  // can't use MockLexicon for WLD ...

    WLDFixture() {
        std::map<std::string, std::string> params;
        params["Lexicon.fsmfile"] = "<dummy>";
        params["Lexicon.symfile"] = "<dummy>";
        params["WLD.paramfile"] = TEST_WEIGHTSFILE;
        lex = new Lexicon();
        try {
            lex->init(params);
        } catch(Norma::Normalizer::init_error e) {}
        lex->add("an");
        lex->add("ja");
        lex->add("in");
        lex->add("ihn");
        lex->add("ihm");
        lex->add("und");
        w = new WLD();
        w->set_name("WLD");
        w->init(params, lex);
    }
    ~WLDFixture() {
        delete w;
        delete lex;
    }
};

BOOST_FIXTURE_TEST_SUITE(WLD1, WLDFixture)

BOOST_AUTO_TEST_CASE(name_check) {
    const std::string name = "WLD";
    BOOST_CHECK_EQUAL(w->name(), name);
}

BOOST_AUTO_TEST_CASE(wld_normalize_1) {
    BOOST_REQUIRE(lex->contains("in"));
    BOOST_REQUIRE(lex->contains("ihm"));
    Result result;
    result = (*w)("in");
    BOOST_CHECK_EQUAL(result.word, "in");
    BOOST_CHECK_CLOSE(result.score, 1.0, 0.001);
    result = (*w)("jn");
    BOOST_CHECK_EQUAL(result.word, "in");
    BOOST_CHECK_CLOSE(result.score, 0.818731, 0.001);
    result = (*w)("jhn");
    BOOST_CHECK_EQUAL(result.word, "ihn");
    BOOST_CHECK_CLOSE(result.score, 0.818731, 0.001);
    result = (*w)("jm");
    BOOST_CHECK_EQUAL(result.word, "ihm");
    BOOST_CHECK_CLOSE(result.score, 0.449329, 0.001);
}

BOOST_AUTO_TEST_CASE(wld_re_init) {
    Result result;
    result = (*w)("jn");
    BOOST_REQUIRE_EQUAL(result.word, "in");
    BOOST_REQUIRE_CLOSE(result.score, 0.818731, 0.001);
    // re-initialization shouldn't change results
    w->init();
    result = (*w)("jn");
    BOOST_CHECK_EQUAL(result.word, "in");
    BOOST_CHECK_CLOSE(result.score, 0.818731, 0.001);
}

BOOST_AUTO_TEST_CASE(wld_normalize_2) {
    BOOST_REQUIRE(lex->contains("und"));
    Result result;
    result = (*w)("und");
    BOOST_CHECK_EQUAL(result.word, "und");
    BOOST_CHECK_CLOSE(result.score, 1.0, 0.001);
    result = (*w)("unnd");
    BOOST_CHECK_EQUAL(result.word, "und");
    BOOST_CHECK_CLOSE(result.score, 0.606531, 0.001);
}

BOOST_AUTO_TEST_CASE(wld_normalize_3) {
    BOOST_REQUIRE(lex->contains("ja"));
    Result result;
    result = (*w)("ja");
    BOOST_CHECK_EQUAL(result.word, "ja");
    BOOST_CHECK_CLOSE(result.score, 1.0, 0.001);
    result = (*w)("jxa");
    BOOST_CHECK_EQUAL(result.word, "ja");
    BOOST_CHECK_CLOSE(result.score, 0.371577, 0.001);
    result = (*w)("jaf");
    BOOST_CHECK_EQUAL(result.word, "ja");
    BOOST_CHECK_CLOSE(result.score, 0.367879, 0.001);
}

BOOST_AUTO_TEST_CASE(wld_normalize_4) {
    Result result;
    result = (*w)("j");
    BOOST_CHECK_EQUAL(result.word, "j");
    BOOST_CHECK_EQUAL(result.score, 0);
}

BOOST_AUTO_TEST_CASE(wld_normalize_n_best) {
    ResultSet given = (*w)("jn", 5);
    ResultSet expected {Result("in", 0.818731),
                        Result("ihn", 0.449329),
                        Result("an", 0.367879),
                        Result("ihm", 0.182684)};
    BOOST_REQUIRE_EQUAL(given.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        Result given_result = given[i],
            expected_result = expected[i];
        BOOST_CHECK_EQUAL(given_result.word, expected_result.word);
        BOOST_CHECK_CLOSE(given_result.score, expected_result.score, 0.001);
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(WLD2)

BOOST_AUTO_TEST_CASE(paramless_wld_nolex_noweights) {
    WLD w;
    Result foo;
    w.init();
    BOOST_REQUIRE_NO_THROW(foo = w("foo"));
    BOOST_CHECK_EQUAL(foo.word, "foo");
    BOOST_CHECK_EQUAL(foo.score, 0);
}

BOOST_AUTO_TEST_CASE(paramless_wld_noweights) {
    WLD w;
    Lexicon lex;
    Result foo;
    lex.init();
    w.set_lexicon(&lex);
    w.init();
    BOOST_REQUIRE_NO_THROW(foo = w("foo"));
    BOOST_CHECK_EQUAL(foo.word, "foo");
    BOOST_CHECK_EQUAL(foo.score, 0);
}

BOOST_AUTO_TEST_CASE(paramless_wld_nolex) {
    WLD w;
    Result foo;
    w.set_paramfile(TEST_WEIGHTSFILE).init();
    BOOST_REQUIRE_NO_THROW(foo = w("foo"));
    BOOST_CHECK_EQUAL(foo.word, "foo");
    BOOST_CHECK_EQUAL(foo.score, 0);
}

BOOST_AUTO_TEST_CASE(paramless_wld_emptylex) {
    WLD w;
    Lexicon lex;
    Result foo;
    lex.init();
    w.set_lexicon(&lex);
    w.set_paramfile(TEST_WEIGHTSFILE).init();
    BOOST_REQUIRE_NO_THROW(foo = w("foo"));
    BOOST_CHECK_EQUAL(foo.word, "foo");
    BOOST_CHECK_EQUAL(foo.score, 0);
}

BOOST_AUTO_TEST_CASE(paramless_wld_training) {
    WLD w;
    Lexicon lex;
    Result foo;
    lex.init();
    w.set_lexicon(&lex);
    w.init();
    // training once...
    Norma::TrainingData data;
    data.add_pair("foo", "bar");
    lex.add("bar");
    w.train(&data);
    w.perform_training();  // hack to actually perform training
    BOOST_REQUIRE_NO_THROW(foo = w("foo"));
    BOOST_CHECK_EQUAL(foo.word, "bar");
}

BOOST_AUTO_TEST_CASE(non_existant_filename) {
    WLD w;
    Lexicon lex;
    lex.init();
    w.set_lexicon(&lex);
    w.set_paramfile("<dummy>");
    BOOST_CHECK_THROW(w.init(), Norma::Normalizer::init_error);
}

BOOST_AUTO_TEST_CASE(malformed_paramfile) {
    WLD w;
    Lexicon lex;
    lex.init();
    w.set_lexicon(&lex);
    w.set_paramfile(TEST_MALFORMED_WEIGHTSFILE);
    BOOST_CHECK_THROW(w.init(), Norma::Normalizer::init_error);
}

BOOST_AUTO_TEST_SUITE_END()
