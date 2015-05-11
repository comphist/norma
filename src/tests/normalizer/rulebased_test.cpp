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
#define BOOST_TEST_MODULE Normalizer_Rulebased
#include<algorithm>
#include<initializer_list>
#include<map>
#include<queue>
#include<sstream>
#include<string>
#include<tuple>
#include<vector>
#include"tests/tests.h"
#include"config.h"
#include"mock_lexicon.h"
#include"normalizer/exceptions.h"
#include"normalizer/rulebased.h"
#include"normalizer/rulebased/symbols.h"

using namespace Norma::Normalizer::Rulebased;  // NOLINT[build/namespaces]
using Norma::Normalizer::Result;
using Norma::Normalizer::ResultSet;

const std::string TEST_RULESFILE =
    std::string(TEST_BASE_DIR) + "/test-rulesfile.txt";
const std::string TEST_MALFORMED_RULESFILE =
    std::string(TEST_BASE_DIR) + "/test-rulesfile-malformed.txt";

/////////////// Rule & RuleSet /////////////////////////////////////////////////

struct RuleFixture {
    Rule p = Rule("a", "b", "l", "r");
    Rule q = Rule(Symbols::EPSILON, "xx",
                  from_char(Symbols::BOUNDARY), "y");
    Rule r = Rule("aba", "c",
                  "l", from_char(Symbols::BOUNDARY));
    RuleSet rs;
    const string_impl source = "jnx";
    const string_impl target = "ihn";

    RuleFixture() {}
    ~RuleFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(Rule1, RuleFixture)

BOOST_AUTO_TEST_CASE(rule_getters) {
    BOOST_CHECK_EQUAL(p.from(), "a");
    BOOST_CHECK_EQUAL(p.to(),   "b");
    BOOST_CHECK_EQUAL(p.left(), 'l');
    BOOST_CHECK_EQUAL(p.right(), 'r');
}

BOOST_AUTO_TEST_CASE(rule_type) {
    BOOST_CHECK(p.type() == EditOp::SUB);
    Rule q("a", Symbols::EPSILON, "l", "r");
    BOOST_CHECK(q.type() == EditOp::DEL);
    Rule r(Symbols::EPSILON, "b", "l", "r");
    BOOST_CHECK(r.type() == EditOp::ADD);
    Rule s("a", "a", "l", "r");
    BOOST_CHECK(s.type() == EditOp::IDENT);
    Rule t(Symbols::EPSILON, Symbols::EPSILON, "l", "r");
    BOOST_CHECK(t.type() == EditOp::IDENT);
}

BOOST_AUTO_TEST_CASE(rule_ctor_argument) {
    BOOST_CHECK_THROW(Rule("a", "b", "invalid", "c"),
                      std::invalid_argument);
    BOOST_CHECK_THROW(Rule("a", "b", "c", "invalid"),
                      std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(rule_comparison) {
    Rule p_clone("a", "b", "l", "r");
    BOOST_CHECK(p == p_clone);
    BOOST_CHECK(p != r);
    BOOST_CHECK(p_clone != r);
}

BOOST_AUTO_TEST_CASE(rule_stream) {
    std::stringstream ss;
    ss << p;
    BOOST_CHECK_EQUAL(ss.str(), "{a->b/l_r}");
}

BOOST_AUTO_TEST_CASE(rule_matches_left) {
    BOOST_CHECK(p.matches_left('l'));
    BOOST_CHECK(!p.matches_left('r'));
    BOOST_CHECK(!p.matches_left('a'));
    BOOST_CHECK(!p.matches_left(Symbols::BOUNDARY));

    BOOST_CHECK(q.matches_left(Symbols::BOUNDARY));
    BOOST_CHECK(!q.matches_left('x'));
    BOOST_CHECK(!q.matches_left('y'));
}

BOOST_AUTO_TEST_CASE(rule_matches_right) {
    BOOST_CHECK(p.matches_right('r'));
    BOOST_CHECK(!p.matches_right('l'));
    BOOST_CHECK(!p.matches_right('a'));
    BOOST_CHECK(!p.matches_right(Symbols::BOUNDARY));

    BOOST_CHECK(r.matches_right(Symbols::BOUNDARY));
    BOOST_CHECK(!r.matches_right('a'));
    BOOST_CHECK(!r.matches_right('c'));
    BOOST_CHECK(!r.matches_right('l'));
}

BOOST_AUTO_TEST_CASE(rule_matches_back) {
    BOOST_CHECK(p.matches_back("ar", false));
    BOOST_CHECK(!p.matches_back("ar", true));
    BOOST_CHECK(!p.matches_back("r",  true));
    BOOST_CHECK(p.matches_back("ar" + from_char(Symbols::BOUNDARY), false));
    BOOST_CHECK(p.matches_back("arblahblah", false));
    BOOST_CHECK(!p.matches_back("arblahblah", true));
    BOOST_CHECK(!p.matches_back("rblahblah",  true));
    BOOST_CHECK(!p.matches_back("a", false));
    BOOST_CHECK(!p.matches_back("lar", false));
    BOOST_CHECK(!p.matches_back("", false));
    BOOST_CHECK(!p.matches_back(from_char(Symbols::BOUNDARY), false));

    BOOST_CHECK(q.matches_back("y", true));
    BOOST_CHECK(!q.matches_back("y", false));
    BOOST_CHECK(q.matches_back("yz", true));
    BOOST_CHECK(!q.matches_back("yz", false));
    BOOST_CHECK(q.matches_back("y" + from_char(Symbols::BOUNDARY), true));
    BOOST_CHECK(!q.matches_back("", true));
    BOOST_CHECK(!q.matches_back(from_char(Symbols::BOUNDARY), true));

    BOOST_CHECK(r.matches_back("aba" + from_char(Symbols::BOUNDARY), false));
    BOOST_CHECK(!r.matches_back("aba" + from_char(Symbols::BOUNDARY), true));
    BOOST_CHECK(!r.matches_back("abax" + from_char(Symbols::BOUNDARY), false));
    BOOST_CHECK(!r.matches_back("ba" + from_char(Symbols::BOUNDARY), false));
    BOOST_CHECK(!r.matches_back("a" + from_char(Symbols::BOUNDARY), false));
    BOOST_CHECK(!r.matches_back("aba", false));
    BOOST_CHECK(!r.matches_back(from_char(Symbols::BOUNDARY), false));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(RuleSet1, RuleFixture)

BOOST_AUTO_TEST_CASE(ruleset_length) {
    BOOST_CHECK_EQUAL(rs.slots(), 0);
    BOOST_CHECK_EQUAL(rs.length(), 0);
    rs.add_rule(EditOp::SUB, source, 1, target, 1);
    BOOST_CHECK_EQUAL(rs.slots(), 1);
    BOOST_CHECK_EQUAL(rs.length(), 1);
}

BOOST_AUTO_TEST_CASE(ruleset_cost) {
    BOOST_CHECK_EQUAL(rs.cost(), 0);
    rs.add_rule(EditOp::SUB, source, 1, target, 1);
    BOOST_CHECK_EQUAL(rs.cost(), 1);
    rs.add_rule(EditOp::ADD, source, 1, target, 2);
    BOOST_CHECK_EQUAL(rs.cost(), 2);
    rs.add_rule(EditOp::IDENT, source, 2, target, 3);
    BOOST_CHECK_EQUAL(rs.cost(), 2);
    rs.add_rule(EditOp::DEL, source, 3, target, 3);
    BOOST_CHECK_EQUAL(rs.cost(), 3);
}

BOOST_AUTO_TEST_CASE(ruleset_count) {
    BOOST_CHECK_EQUAL(rs.count(EditOp::SUB), 0);
    BOOST_CHECK_EQUAL(rs.count(EditOp::ADD), 0);
    BOOST_CHECK_EQUAL(rs.count(EditOp::DEL), 0);
    BOOST_CHECK_EQUAL(rs.count(EditOp::IDENT), 0);
    rs.add_rule(EditOp::SUB, source, 1, target, 1);
    BOOST_CHECK_EQUAL(rs.count(EditOp::SUB), 1);
    BOOST_CHECK_EQUAL(rs.count(EditOp::ADD), 0);
    BOOST_CHECK_EQUAL(rs.count(EditOp::DEL), 0);
    BOOST_CHECK_EQUAL(rs.count(EditOp::IDENT), 0);
    rs.add_rule(EditOp::ADD, source, 1, target, 2);
    BOOST_CHECK_EQUAL(rs.count(EditOp::SUB), 1);
    BOOST_CHECK_EQUAL(rs.count(EditOp::ADD), 1);
    BOOST_CHECK_EQUAL(rs.count(EditOp::DEL), 0);
    BOOST_CHECK_EQUAL(rs.count(EditOp::IDENT), 0);
    rs.add_rule(EditOp::IDENT, source, 2, target, 3);
    BOOST_CHECK_EQUAL(rs.count(EditOp::SUB), 1);
    BOOST_CHECK_EQUAL(rs.count(EditOp::ADD), 1);
    BOOST_CHECK_EQUAL(rs.count(EditOp::DEL), 0);
    BOOST_CHECK_EQUAL(rs.count(EditOp::IDENT), 1);
    rs.add_rule(EditOp::DEL, source, 3, target, 3);
    BOOST_CHECK_EQUAL(rs.count(EditOp::SUB), 1);
    BOOST_CHECK_EQUAL(rs.count(EditOp::ADD), 1);
    BOOST_CHECK_EQUAL(rs.count(EditOp::DEL), 1);
    BOOST_CHECK_EQUAL(rs.count(EditOp::IDENT), 1);
}

BOOST_AUTO_TEST_CASE(ruleset_example) {
    rs.add_rule(EditOp::SUB, source, 1, target, 1);
    BOOST_CHECK_EQUAL(rs.at(0), Rule("j", "i",
                                     Symbols::BOUNDARY, Symbols::BOUNDARY));

    rs.add_rule(EditOp::ADD, source, 1, target, 2);
    BOOST_CHECK_EQUAL(rs.at(0), Rule("j", "i",
                                     Symbols::BOUNDARY, Symbols::BOUNDARY));
    BOOST_CHECK_EQUAL(rs.at(1), Rule(Symbols::EPSILON, "h",
                                     "i", from_char(Symbols::BOUNDARY)));

    rs.add_rule(EditOp::IDENT, source, 2, target, 3);
    BOOST_CHECK_EQUAL(rs.at(1), Rule(Symbols::EPSILON, "h", "i", "n"));
    BOOST_CHECK_EQUAL(rs.at(2), Rule("n", "n",
                                     "h", from_char(Symbols::BOUNDARY)));

    rs.add_rule(EditOp::DEL, source, 3, target, 3);
    BOOST_CHECK_EQUAL(rs.at(2), Rule("n", "n", "h", "x"));
    BOOST_CHECK_EQUAL(rs.at(3), Rule("x", Symbols::EPSILON,
                                     "n", from_char(Symbols::BOUNDARY)));
}

BOOST_AUTO_TEST_SUITE_END()

/////////////// RuleCollection /////////////////////////////////////////////////

struct RuleCollectionEmptyFixture {
    RuleCollection rules;

    RuleCollectionEmptyFixture() {}
    ~RuleCollectionEmptyFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(RuleCollection1, RuleCollectionEmptyFixture)

BOOST_AUTO_TEST_CASE(counts_after_learn_rule) {
    BOOST_CHECK_EQUAL(rules.get_type_count(), 0);
    BOOST_CHECK_EQUAL(rules.get_instance_count(), 0);
    BOOST_CHECK_EQUAL(rules.get_highest_freq(), 0);

    Rule r("a", "b", "l", "r");
    rules.learn_rule(r);
    BOOST_CHECK_EQUAL(rules.get_type_count(), 1);
    BOOST_CHECK_EQUAL(rules.get_instance_count(), 1);
    BOOST_CHECK_EQUAL(rules.get_highest_freq(), 1);

    Rule q("c", "d", "x", "y");
    rules.learn_rule(q, 4);
    BOOST_CHECK_EQUAL(rules.get_type_count(), 2);
    BOOST_CHECK_EQUAL(rules.get_instance_count(), 5);
    BOOST_CHECK_EQUAL(rules.get_highest_freq(), 4);

    // type count shouldn't increment when learning known rules
    rules.learn_rule(q, 3);
    BOOST_CHECK_EQUAL(rules.get_type_count(), 2);
    BOOST_CHECK_EQUAL(rules.get_instance_count(), 8);
    BOOST_CHECK_EQUAL(rules.get_highest_freq(), 7);

    Rule p("a", "b", "x", "y");
    rules.learn_rule(p, 2);
    BOOST_CHECK_EQUAL(rules.get_type_count(), 3);
    BOOST_CHECK_EQUAL(rules.get_instance_count(), 10);
    BOOST_CHECK_EQUAL(rules.get_highest_freq(), 7);
}

BOOST_AUTO_TEST_CASE(counts_after_learn_ruleset) {
    const string_impl source = "jnx";
    const string_impl target = "ihn";
    RuleSet rs;
    rs.add_rule(EditOp::SUB, source, 1, target, 1);
    rs.add_rule(EditOp::ADD, source, 1, target, 2);
    rs.add_rule(EditOp::IDENT, source, 2, target, 3);
    rs.add_rule(EditOp::DEL, source, 3, target, 3);

    rules.learn_ruleset(rs);
    BOOST_CHECK_EQUAL(rules.get_type_count(), 4);
    BOOST_CHECK_EQUAL(rules.get_instance_count(), 4);
    BOOST_CHECK_EQUAL(rules.get_highest_freq(), 1);
}

BOOST_AUTO_TEST_CASE(read_rules_from_file) {
    BOOST_REQUIRE(rules.read_rulesfile(TEST_RULESFILE));
    BOOST_CHECK_EQUAL(rules.get_type_count(), 14);
    BOOST_CHECK_EQUAL(rules.get_highest_freq(), 100000);
    BOOST_CHECK_EQUAL(rules.get_instance_count(), 354855);
    BOOST_CHECK_EQUAL(rules.get_average_freq(), 25346);
    BOOST_CHECK_EQUAL(rules.get_freq(Rule("v", "u", "#", "n")), 1000);
    BOOST_CHECK_EQUAL(rules.get_freq(Rule("e", "ä", "g", "b")), 5);
}

BOOST_AUTO_TEST_SUITE_END()

struct RuleCollectionSampleFixture {
    RuleCollection rules;
    Rule a = Rule("v", "u", "#", "n");
    Rule b = Rule("u", "v", "#", "n");
    Rule c = Rule("vn", "un", "#", "d");
    Rule d = Rule("v", "u", "x", "n");
    Rule e = Rule("v", "v", "#", "n");
    Rule f = Rule("E", "E", "#", "v");
    Rule g = Rule("E", "h", "#", "v");
    Rule h = Rule("E", "E", "#", "n");

    bool vector_equal(std::vector<Rule>* x, std::vector<Rule>* y) {
        std::sort(x->begin(), x->end());
        std::sort(y->begin(), y->end());
        return std::equal(x->begin(), x->end(), y->begin());
    }

    RuleCollectionSampleFixture() {
        for (auto rule : {a, b, c, d, e, f, g, h}) {
            rules.learn_rule(rule);
        }
    }
    ~RuleCollectionSampleFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(RuleCollection2, RuleCollectionSampleFixture)

BOOST_AUTO_TEST_CASE(find_applicable_rules_1) {
    std::vector<Rule> expected {a, c, e};
    std::vector<Rule> result = rules.find_applicable_rules("#", "vnd#", false);
    BOOST_CHECK(vector_equal(&expected, &result));
}

BOOST_AUTO_TEST_CASE(find_applicable_rules_2) {
    std::vector<Rule> expected {f, g};
    std::vector<Rule> result = rules.find_applicable_rules("#", "vnd#", true);
    BOOST_CHECK(vector_equal(&expected, &result));
}

BOOST_AUTO_TEST_CASE(find_applicable_rules_3) {
    std::vector<Rule> expected {a, e};
    std::vector<Rule> result = rules.find_applicable_rules("#",
                                                           "vnter#", false);
    BOOST_CHECK(vector_equal(&expected, &result));
}

BOOST_AUTO_TEST_CASE(rulecollection_clear) {
    BOOST_CHECK_EQUAL(rules.get_type_count(), 8);
    BOOST_CHECK_EQUAL(rules.get_instance_count(), 8);
    BOOST_CHECK_EQUAL(rules.get_highest_freq(), 1);
    rules.clear();
    BOOST_CHECK_EQUAL(rules.get_type_count(), 0);
    BOOST_CHECK_EQUAL(rules.get_instance_count(), 0);
    BOOST_CHECK_EQUAL(rules.get_highest_freq(), 0);
}

BOOST_AUTO_TEST_SUITE_END()

/////////////// CandidateFinder ////////////////////////////////////////////////

// test with minimal rule collection and lexicon
struct CandidateFinderFixture {
    MockLexicon lex;
    RuleCollection rules;
    std::vector<Rule> vnd_rules;

    CandidateFinderFixture() {
        // rules for vnd -> und
        vnd_rules.push_back(Rule("E", "E", "#", "v"));
        vnd_rules.push_back(Rule("v", "u", "#", "n"));
        vnd_rules.push_back(Rule("E", "E", "u", "n"));
        vnd_rules.push_back(Rule("n", "n", "u", "d"));
        vnd_rules.push_back(Rule("E", "E", "n", "d"));
        vnd_rules.push_back(Rule("d", "d", "n", "#"));
        vnd_rules.push_back(Rule("E", "E", "d", "#"));
        // learn rules
        for (const Rule& vnd_rule : vnd_rules)
            rules.learn_rule(vnd_rule, 1);
        // distractor rules
        rules.learn_rule(Rule("v", "v", "#", "n"), 2);
        rules.learn_rule(Rule("E", "E", "v", "n"), 1);
        rules.learn_rule(Rule("n", "n", "v", "d"), 1);
        // make "correct" rule more frequent than distractor rule
        rules.learn_rule(Rule("v", "u", "#", "n"), 9);
    }
    ~CandidateFinderFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(CandidateFinder1, CandidateFinderFixture)

BOOST_AUTO_TEST_CASE(candidate_finder_vnd) {
    // check
    CandidateFinder finder("vnd", rules, lex);
    Result result = finder();
    BOOST_CHECK_EQUAL(result.word, "und");
    BOOST_CHECK_CLOSE(result.score, 0.277777778, 0.001);
}

BOOST_AUTO_TEST_CASE(candidate_finder_vnt) {
    CandidateFinder finder("vnt", rules, lex);
    Result result = finder();
    BOOST_CHECK_EQUAL(result.word, "vnt");
    BOOST_CHECK_CLOSE(result.score, 0.0, 0.001);
}

BOOST_AUTO_TEST_SUITE_END()

/////////////// Rule learning //////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE(RuleLearner)

BOOST_AUTO_TEST_CASE(rule_learn_nomerge_noeps) {
    const string_impl source = "jn";
    const string_impl target = "in";
    RuleSet rs = learn_rules(source, target, false, false);
    BOOST_REQUIRE_EQUAL(rs.length(), 2);
    BOOST_CHECK_EQUAL(rs.at(0), Rule("j", "i",
                      from_char(Symbols::BOUNDARY), "n"));
    BOOST_CHECK_EQUAL(rs.at(1), Rule("n", "n",
                                     "i", from_char(Symbols::BOUNDARY)));
}

BOOST_AUTO_TEST_CASE(rule_learn_nomerge_eps) {
    const string_impl source = "jn";
    const string_impl target = "in";
    RuleSet rs = learn_rules(source, target, false, true);
    BOOST_REQUIRE_EQUAL(rs.length(), 5);
    BOOST_CHECK_EQUAL(rs.at(0), Rule(Symbols::EPSILON, Symbols::EPSILON,
                                     from_char(Symbols::BOUNDARY), "j"));
    BOOST_CHECK_EQUAL(rs.at(1), Rule("j", "i",
                                     from_char(Symbols::BOUNDARY), "n"));
    BOOST_CHECK_EQUAL(rs.at(2), Rule(Symbols::EPSILON, Symbols::EPSILON,
                                     "i", "n"));
    BOOST_CHECK_EQUAL(rs.at(3), Rule("n", "n",
                                     "i", from_char(Symbols::BOUNDARY)));
    BOOST_CHECK_EQUAL(rs.at(4), Rule(Symbols::EPSILON, Symbols::EPSILON,
                                     "n", from_char(Symbols::BOUNDARY)));
}

BOOST_AUTO_TEST_CASE(rule_learn_merge_noeps) {
    const string_impl source = "jn";
    const string_impl target = "ihn";
    RuleSet rs = learn_rules(source, target, true, false);
    BOOST_REQUIRE_EQUAL(rs.length(), 2);
    BOOST_CHECK_EQUAL(rs.at(0), Rule("j", "ih",
                                     from_char(Symbols::BOUNDARY), "n"));
    BOOST_CHECK_EQUAL(rs.at(1), Rule("n", "n",
                                     "h", from_char(Symbols::BOUNDARY)));
}

BOOST_AUTO_TEST_CASE(rule_learn_merge_eps_insertion) {
    const string_impl source = "jn";
    const string_impl target = "ihn";
    RuleSet rs = learn_rules(source, target, true, true);
    BOOST_REQUIRE_EQUAL(rs.length(), 5);
    BOOST_CHECK_EQUAL(rs.at(0), Rule(Symbols::EPSILON, Symbols::EPSILON,
                                     from_char(Symbols::BOUNDARY), "j"));
    BOOST_CHECK_EQUAL(rs.at(1), Rule("j", "ih",
                                     from_char(Symbols::BOUNDARY), "n"));
    BOOST_CHECK_EQUAL(rs.at(2), Rule(Symbols::EPSILON, Symbols::EPSILON,
                                     "h", "n"));
    BOOST_CHECK_EQUAL(rs.at(3), Rule("n", "n",  "h",
                                     from_char(Symbols::BOUNDARY)));
    BOOST_CHECK_EQUAL(rs.at(4), Rule(Symbols::EPSILON, Symbols::EPSILON,
                                     "n", from_char(Symbols::BOUNDARY)));
}

BOOST_AUTO_TEST_CASE(rule_learn_merge_eps_deletion) {
    const string_impl source = "ihn";
    const string_impl target = "jn";
    RuleSet rs = learn_rules(source, target, true, true);
    BOOST_REQUIRE_EQUAL(rs.length(), 5);
    BOOST_CHECK_EQUAL(rs.at(0), Rule(Symbols::EPSILON,  Symbols::EPSILON,
                                     from_char(Symbols::BOUNDARY), "i"));
    BOOST_CHECK_EQUAL(rs.at(1), Rule("ih", "j",
                                     from_char(Symbols::BOUNDARY), "n"));
    BOOST_CHECK_EQUAL(rs.at(2), Rule(Symbols::EPSILON,  Symbols::EPSILON,
                                     "j", "n"));
    BOOST_CHECK_EQUAL(rs.at(3), Rule("n",  "n",
                                     "j", from_char(Symbols::BOUNDARY)));
    BOOST_CHECK_EQUAL(rs.at(4), Rule(Symbols::EPSILON,  Symbols::EPSILON,
                                     "n", from_char(Symbols::BOUNDARY)));
}

BOOST_AUTO_TEST_SUITE_END()

/////////////// Rulebased //////////////////////////////////////////////////////

struct RulebasedFixture {
    Rulebased *r;
    MockLexicon *lex;

    RulebasedFixture() {
        std::map<std::string, std::string> params;
        params["RuleBased.rulesfile"] = TEST_RULESFILE;
        lex = new MockLexicon();
        r = new Rulebased();
        r->set_name("RuleBased");
        r->init(params, lex);
    }
    ~RulebasedFixture() {
        delete r;
        delete lex;
    }
};

BOOST_FIXTURE_TEST_SUITE(Rulebased1, RulebasedFixture)

BOOST_AUTO_TEST_CASE(name_check) {
    const std::string name = "RuleBased";
    BOOST_CHECK_EQUAL(r->name(), name);
}

BOOST_AUTO_TEST_CASE(rulebased_normalize_best) {
    Result vnd = (*r)("vnd");
    BOOST_CHECK_EQUAL(vnd.word, "und");
    BOOST_CHECK(vnd.score > 0);
    Result foo = (*r)("fvo");
    BOOST_CHECK_EQUAL(foo.word, "fvo");
    BOOST_CHECK_EQUAL(foo.score, 0);
}

BOOST_AUTO_TEST_CASE(rulebased_normalize_n_best) {
    ResultSet given = (*r)("vnd", 5);
    ResultSet expected {Result("und", 0),  // score not checked
                        Result("eins", 0)};
    BOOST_REQUIRE_EQUAL(given.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        Result given_result = given[i],
            expected_result = expected[i];
        BOOST_CHECK_EQUAL(given_result.word, expected_result.word);
    }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Rulebased2)

BOOST_AUTO_TEST_CASE(paramless_rulebased) {
    Rulebased r;
    r.init();
    Result foo = r("foo");
    BOOST_CHECK_EQUAL(foo.word, "foo");
    BOOST_CHECK_EQUAL(foo.score, 0);
}

BOOST_AUTO_TEST_CASE(non_existant_filename) {
    Rulebased r;
    r.set_rulesfile("<dummy>");
    BOOST_CHECK_THROW(r.init(), Norma::Normalizer::init_error);
}

BOOST_AUTO_TEST_CASE(malformed_paramfile) {
    Rulebased r;
    r.set_rulesfile(TEST_MALFORMED_RULESFILE);
    BOOST_CHECK_THROW(r.init(), Norma::Normalizer::init_error);
}

BOOST_AUTO_TEST_SUITE_END()
