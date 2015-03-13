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
#define BOOST_TEST_MODULE Gfsm_Wrapper
#include<map>
#include<string>
#include<iostream>
#include<stdexcept>
#include<set>
#include<tuple>
#include<vector>
#include<boost/test/included/unit_test.hpp> // NOLINT[build/include_order]
#include"gfsm_wrapper.h"
#include"string_impl.h"
#include"config.h"

// we are basically testing *everything* in the namespace here...
using namespace Gfsm; // NOLINT[build/namespaces]

/* Note: Most tests of the more sophisticated components (e.g.,
   StringTransducer, Cascade, etc.) are not really independent of the
   previous tests; they use the existing components rather than mock
   objects. */

/////////////////////// LabelVector ///////////////////////

struct GfsmLabelVectorFixture {
    LabelVector vec {42, 43, 44};

    GfsmLabelVectorFixture() { }
    ~GfsmLabelVectorFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(LabelVector1, GfsmLabelVectorFixture)

BOOST_AUTO_TEST_CASE(labelvector_get) {
    BOOST_CHECK_EQUAL(vec.get(0), 42);
    BOOST_CHECK_EQUAL(vec.get(1), 43);
    BOOST_CHECK_EQUAL(vec.get(2), 44);
    BOOST_CHECK_THROW(vec.get(3), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(labelvector_equality) {
    LabelVector x {42, 43, 44};
    LabelVector y {44, 43, 42};
    BOOST_CHECK(vec == vec);
    BOOST_CHECK(vec == x);
    BOOST_CHECK(vec != y);
    BOOST_CHECK(x == vec);
    BOOST_CHECK(y != vec);
}

BOOST_AUTO_TEST_CASE(labelvector_less) {
    LabelVector id {42, 43, 44};
    LabelVector x {42, 43, 42};
    LabelVector y {41, 43, 44};
    LabelVector z {44, 41, 40};
    LabelVector s {42, 43};
    LabelVector empty;
    BOOST_CHECK(!(vec < vec));
    BOOST_CHECK(!(vec < id));
    BOOST_CHECK(!(id < vec));
    BOOST_CHECK(x < vec);
    BOOST_CHECK(y < vec);
    BOOST_CHECK(!(vec < x));
    BOOST_CHECK(!(vec < y));
    BOOST_CHECK(vec < z);
    BOOST_CHECK(!(z < vec));
    BOOST_CHECK(s < x);
    BOOST_CHECK(!(x < s));
    BOOST_CHECK(empty < vec);
    BOOST_CHECK(!(vec < empty));
    BOOST_CHECK(!(empty < empty));
}

BOOST_AUTO_TEST_CASE(labelvector_size) {
    BOOST_CHECK_EQUAL(vec.size(), 3);
}

BOOST_AUTO_TEST_CASE(labelvector_clear) {
    vec.clear();
    BOOST_CHECK_EQUAL(vec.size(), 0);
    BOOST_CHECK_THROW(vec.get(0), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(labelvector_copy) {
    LabelVector vec_copy(vec);
    BOOST_CHECK(vec == vec_copy);
    BOOST_CHECK(!(vec < vec_copy) && !(vec_copy < vec));
    vec_copy.push_back(99);
    BOOST_CHECK_EQUAL(vec.size(), 3);
    BOOST_CHECK_EQUAL(vec_copy.size(), 4);
}

BOOST_AUTO_TEST_CASE(labelvector_assign) {
    LabelVector vec_copy;
    vec_copy.push_back(99);
    BOOST_CHECK_EQUAL(vec.size(), 3);
    vec = vec_copy;
    BOOST_CHECK(vec == vec_copy);
    BOOST_CHECK(!(vec < vec_copy) && !(vec_copy < vec));
    BOOST_CHECK_EQUAL(vec.size(), 1);
    vec.push_back(100);
    BOOST_CHECK_EQUAL(vec.size(), 2);
    BOOST_CHECK_EQUAL(vec_copy.size(), 1);
}

BOOST_AUTO_TEST_SUITE_END()

//////////////////////// Alphabet /////////////////////////

struct GfsmAlphabetFixture {
    Alphabet alph;
    const std::string INVALID_FILE =
        std::string(TEST_BASE_DIR) + "/nonexistent";
    const std::string LABELS_FILE =
        std::string(TEST_BASE_DIR) + "/test-lexicon.lab";

    GfsmAlphabetFixture() {
        alph.add_mapping("<eps>", 1);
        alph.add_mapping("ß", 20);
        alph.add_mapping("d", 42);
        alph.add_mapping("n", 43);
        alph.add_mapping("u", 44);
    }
    ~GfsmAlphabetFixture() {}
};

BOOST_FIXTURE_TEST_SUITE(Alphabet1, GfsmAlphabetFixture)

BOOST_AUTO_TEST_CASE(contains_symbol) {
    BOOST_CHECK(alph.contains("u"));
    BOOST_CHECK(alph.contains("<eps>"));
    BOOST_CHECK(!(alph.contains("a")));
}

BOOST_AUTO_TEST_CASE(contains_label) {
    BOOST_CHECK(alph.contains(1));
    BOOST_CHECK(alph.contains(42));
    BOOST_CHECK(!(alph.contains(99)));
}

BOOST_AUTO_TEST_CASE(get_label) {
    BOOST_CHECK_EQUAL(alph.get_label("ß"), 20);
    BOOST_CHECK_EQUAL(alph.get_label("u"), 44);
    BOOST_CHECK_EQUAL(alph.get_label("<eps>"), 1);
    BOOST_CHECK_THROW(alph.get_label("a"), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(get_symbol) {
    BOOST_CHECK_EQUAL(to_cstr(alph.get_symbol(44)), "u");
    BOOST_CHECK_EQUAL(to_cstr(alph.get_symbol(1)), "<eps>");
    BOOST_CHECK_THROW(alph.get_symbol(99), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(map_symbol) {
    BOOST_CHECK_EQUAL(alph.map_symbol("u"), 44);
    BOOST_CHECK_EQUAL(alph.map_symbol("<eps>"), 1);
    BOOST_CHECK_EQUAL(alph.map_symbol("a"), 0);
    BOOST_CHECK_EQUAL(alph.map_symbol("ß"), 20);
}

BOOST_AUTO_TEST_CASE(alphabet_covered) {
    std::set<string_impl> results = alph.covered();
    std::set<string_impl> expected
        {"u", "n", "d", "ß", "<eps>"};
    BOOST_CHECK(results == expected);
}

BOOST_AUTO_TEST_CASE(unknown_mapper) {
    auto my_mapper = [](const string_impl& s) -> const string_impl {
        // maps "a" to "u"
        if (s == "a") {
            return "u";
        }
        return s;
    };
    alph.set_unknown_mapper(my_mapper);
    BOOST_CHECK_EQUAL(alph.map_symbol("u"), 44);
    BOOST_CHECK_EQUAL(alph.map_symbol("<eps>"), 1);
    BOOST_CHECK_EQUAL(alph.map_symbol("a"), 44);
    // "e" is still not mapped to anything legal
    BOOST_CHECK_EQUAL(alph.map_symbol("e"), 0);
}

BOOST_AUTO_TEST_CASE(map_symbols1) {
    const string_impl my_str = "und";
    LabelVector vec = alph.map_symbols(my_str);
    BOOST_REQUIRE_EQUAL(vec.size(), 3);
    BOOST_CHECK_EQUAL(vec.get(0), 44);
    BOOST_CHECK_EQUAL(vec.get(1), 43);
    BOOST_CHECK_EQUAL(vec.get(2), 42);
}

BOOST_AUTO_TEST_CASE(map_symbols2) {
    std::vector<string_impl> my_str {"u", "<eps>", "ß"};
    LabelVector vec = alph.map_symbols(my_str);
    BOOST_REQUIRE_EQUAL(vec.size(), 3);
    BOOST_CHECK_EQUAL(vec.get(0), 44);
    BOOST_CHECK_EQUAL(vec.get(1),  1);
    BOOST_CHECK_EQUAL(vec.get(2), 20);
}

BOOST_AUTO_TEST_CASE(add_symbol) {
    BOOST_CHECK(!alph.contains("ä"));
    BOOST_CHECK_EQUAL(alph.add_symbol("ä"), 45);
    BOOST_CHECK(!alph.contains("ö"));
    BOOST_CHECK_EQUAL(alph.add_symbol("ö"), 46);
    alph.add_mapping("z", 101);
    BOOST_CHECK(!alph.contains("ü"));
    BOOST_CHECK_EQUAL(alph.add_symbol("ü"), 102);
}

BOOST_AUTO_TEST_CASE(cover1) {
    const string_impl my_str = "naß";
    LabelVector vec = alph.map_symbols(my_str);
    BOOST_REQUIRE_EQUAL(vec.size(), my_str.length());
    for (string_size i = 0; i < my_str.length(); ++i) {
        string_impl s = from_char(my_str[i]);
        BOOST_CHECK_EQUAL(vec.get(i), alph.map_symbol(s));
    }
    BOOST_CHECK_EQUAL(alph.map_symbol("a"), 0);  // "a" is unknown
    vec = alph.cover(my_str);
    BOOST_REQUIRE_EQUAL(vec.size(), my_str.length());
    for (string_size i = 0; i < my_str.length(); ++i) {
        string_impl s = from_char(my_str[i]);
        BOOST_CHECK_EQUAL(vec.get(i), alph.map_symbol(s));
        BOOST_CHECK(alph.map_symbol(s) > 0);  // no unknown characters
    }
}

BOOST_AUTO_TEST_CASE(cover2) {
    std::vector<string_impl> my_str = {"n", "a", "ß"};
    LabelVector vec = alph.map_symbols(my_str);
    BOOST_REQUIRE_EQUAL(vec.size(), 3);
    BOOST_CHECK_EQUAL(vec.get(0), 43);
    BOOST_CHECK_EQUAL(vec.get(1), 0);  // "a" is unknown
    BOOST_CHECK_EQUAL(vec.get(2), 20);
    vec = alph.cover(my_str);
    BOOST_REQUIRE_EQUAL(vec.size(), 3);
    BOOST_CHECK_EQUAL(vec.get(0), 43);
    BOOST_CHECK_EQUAL(vec.get(1), 45);  // learned "a"
    BOOST_CHECK_EQUAL(vec.get(2), 20);
}

BOOST_AUTO_TEST_CASE(ignore_unknowns) {
    const string_impl my_str = "ende";
    LabelVector vec;
    vec = alph.map_symbols(my_str);
    BOOST_REQUIRE_EQUAL(vec.size(), 4);
    BOOST_CHECK_EQUAL(vec.get(0), 0);
    BOOST_CHECK_EQUAL(vec.get(1), 43);
    BOOST_CHECK_EQUAL(vec.get(2), 42);
    BOOST_CHECK_EQUAL(vec.get(3), 0);
    // ignore unknowns only maps known characters:
    alph.set_ignore_unknowns(true);
    vec = alph.map_symbols(my_str);
    BOOST_REQUIRE_EQUAL(vec.size(), 2);
    BOOST_CHECK_EQUAL(vec.get(0), 43);
    BOOST_CHECK_EQUAL(vec.get(1), 42);
}

BOOST_AUTO_TEST_CASE(map_labels) {
    LabelVector vec;
    vec.push_back(44);
    vec.push_back(43);
    vec.push_back(42);
    string_impl my_str = alph.map_labels(vec);
    BOOST_CHECK_EQUAL(to_cstr(my_str), "und");
}

BOOST_AUTO_TEST_CASE(map_labels_to_vector) {
    std::vector<string_impl> expected {"u", "<eps>", "ß"};
    std::vector<string_impl> my_str;
    LabelVector vec;
    vec.push_back(44);
    vec.push_back(1);
    vec.push_back(20);
    my_str = alph.map_labels_to_vector(vec);
    BOOST_CHECK(my_str == expected);
}

BOOST_AUTO_TEST_CASE(read_from_labfile) {
    BOOST_REQUIRE_NO_THROW(alph.load_labfile(LABELS_FILE));
    BOOST_CHECK(alph.contains("<#>"));
    BOOST_CHECK(alph.contains("a"));
    BOOST_CHECK(alph.contains("ü"));
    BOOST_CHECK(alph.contains(19));
    BOOST_CHECK(!(alph.contains("ß")));
    BOOST_CHECK(!(alph.contains(21)));
}

BOOST_AUTO_TEST_CASE(load_nonexistent_file) {
    BOOST_CHECK_THROW(alph.load_labfile(INVALID_FILE),
                      std::runtime_error);
}

BOOST_AUTO_TEST_SUITE_END()

//////////////////////// Automaton ////////////////////////

struct GfsmAutomatonFixture {
    Automaton* fsm;
    Gfsm::AutomatonBuilder& _builder = Gfsm::AutomatonBuilder::instance();

    GfsmAutomatonFixture() {
        fsm = new Automaton(_builder.make_automaton());
    }
    ~GfsmAutomatonFixture() {
        delete fsm;
    }
};

BOOST_FIXTURE_TEST_SUITE(Automaton1, GfsmAutomatonFixture)

BOOST_AUTO_TEST_CASE(semiring_type) {
    // default should be tropical
    BOOST_CHECK(fsm->get_semiring_type() == SemiringType::TROPICAL);
    // set to something else
    fsm->set_semiring_type(SemiringType::ARCTIC);
    BOOST_CHECK(fsm->get_semiring_type() == SemiringType::ARCTIC);
}

BOOST_AUTO_TEST_SUITE_END()


//////////////////////// Acceptor ////////////////////////

struct GfsmAcceptorFixture {
    Acceptor* fsm;
    Alphabet  alph;
    StringAcceptor* sa;
    Gfsm::AutomatonBuilder& _builder = Gfsm::AutomatonBuilder::instance();
    const std::string LEXICON_FILE =
        std::string(TEST_BASE_DIR) + "/test-lexicon.gfsa";
    const std::string LABELS_FILE =
        std::string(TEST_BASE_DIR) + "/test-lexicon.lab";
    const LabelVector eins {4, 5, 6, 7};
    const LabelVector zwei {8, 9, 4, 5};
    const LabelVector zwei_term {8, 9, 4, 5, 1};
    const LabelVector zweite {8, 9, 4, 5, 19, 4};
    const LabelVector zweite_term {8, 9, 4, 5, 19, 4, 1};
    const LabelVector zei {8, 4, 5};
    const LabelVector zeit {8, 4, 5, 19};
    const LabelVector zeit_term {8, 4, 5, 19, 1};
    const LabelVector invalid_labels {44, 49, 20};

    GfsmAcceptorFixture() {
        fsm = new Acceptor(_builder.make_acceptor());
        sa = new StringAcceptor(_builder.make_stringacceptor());

        fsm->load_binfile(LEXICON_FILE);
        alph.load_labfile(LABELS_FILE);
        sa->load_binfile(LEXICON_FILE);
        sa->set_alphabet(alph);
    }
    ~GfsmAcceptorFixture() {
        delete fsm;
        delete sa;
    }
};

BOOST_FIXTURE_TEST_SUITE(Acceptor1, GfsmAcceptorFixture)

BOOST_AUTO_TEST_CASE(accepts) {
    BOOST_CHECK(fsm->accepts(eins));
    BOOST_CHECK(fsm->accepts(zwei));
    BOOST_CHECK(fsm->accepts(zweite));
    BOOST_CHECK(fsm->accepts(zwei_term));
    BOOST_CHECK(!fsm->accepts(zweite_term));
    BOOST_CHECK(!fsm->accepts(invalid_labels));
}

BOOST_AUTO_TEST_CASE(accepted) {
    std::set<LabelVector> a = fsm->accepted();
    BOOST_CHECK(a.count(eins) > 0);
    BOOST_CHECK(a.count(zwei) > 0);
    BOOST_CHECK(a.count(zweite) > 0);
    BOOST_CHECK(a.count(zwei_term) > 0);
    BOOST_CHECK(a.count(zweite_term) == 0);
    BOOST_CHECK(a.count(invalid_labels) == 0);
    BOOST_CHECK_EQUAL(a.size(), 59);
}

BOOST_AUTO_TEST_CASE(add_path) {
    BOOST_CHECK(!fsm->accepts(zweite_term));
    fsm->add_path(zweite_term);
    BOOST_CHECK(fsm->accepts(zweite_term));
    // adding an already accepted word should change nothing
    BOOST_CHECK(fsm->accepts(eins));
    fsm->add_path(eins);
    BOOST_CHECK(fsm->accepts(eins));
    // others should be unaffected
    BOOST_CHECK(fsm->accepts(zwei));
    BOOST_CHECK(fsm->accepts(zweite));
    BOOST_CHECK(fsm->accepts(zwei_term));
    BOOST_CHECK(fsm->accepts(zweite_term));
    BOOST_CHECK(!fsm->accepts(invalid_labels));
}

BOOST_AUTO_TEST_CASE(add_path_set_all_final) {
    BOOST_REQUIRE(!fsm->accepts(zeit_term));
    fsm->add_path(zeit_term);
    BOOST_CHECK(fsm->accepts(zeit_term));
    BOOST_CHECK(!fsm->accepts(zeit));
    BOOST_CHECK(!fsm->accepts(zei));
    fsm->add_path(zeit_term, true);
    BOOST_CHECK(fsm->accepts(zeit_term));
    BOOST_CHECK(fsm->accepts(zeit));
    BOOST_CHECK(fsm->accepts(zei));
}

BOOST_AUTO_TEST_CASE(add_path_on_empty) {
    Acceptor fsm1 = _builder.make_acceptor();
    BOOST_CHECK(!fsm1.accepts(eins));
    BOOST_CHECK(!fsm1.accepts(zwei));
    BOOST_CHECK(!fsm1.accepts(zei));
    fsm1.add_path(eins);
    fsm1.add_path(zei);
    fsm1.add_path(zwei);
    BOOST_CHECK(fsm1.accepts(eins));
    BOOST_CHECK(fsm1.accepts(zwei));
    BOOST_CHECK(fsm1.accepts(zei));
    Acceptor fsm2 = _builder.make_acceptor();
    BOOST_CHECK(!fsm2.accepts(eins));
    BOOST_CHECK(!fsm2.accepts(zwei));
    BOOST_CHECK(!fsm2.accepts(zei));
    fsm2.add_path(zwei);
    fsm2.add_path(eins);
    fsm2.add_path(zei);
    BOOST_CHECK(fsm2.accepts(eins));
    BOOST_CHECK(fsm2.accepts(zwei));
    BOOST_CHECK(fsm2.accepts(zei));
}

// StringAcceptor might get its own test suite later
BOOST_AUTO_TEST_CASE(string_acceptor_inheritance) {
    BOOST_CHECK(sa->accepts(eins));
    StringAcceptor sb(*sa);
    BOOST_CHECK(sb.accepts(eins));
}

BOOST_AUTO_TEST_CASE(string_acceptor) {
    BOOST_CHECK(sa->accepts("eins"));
    const std::vector<string_impl> v1 {"e", "i", "n", "s"};
    BOOST_CHECK(sa->accepts(v1));
    BOOST_CHECK(sa->accepts("zweite"));
    const std::vector<string_impl> v2 {"z", "w", "e", "i", "<#>"};
    BOOST_CHECK(sa->accepts(v2));
    const std::vector<string_impl> v3 {"z", "w", "e", "i", "t", "e", "<#>"};
    BOOST_CHECK(!sa->accepts(v3));
}

BOOST_AUTO_TEST_CASE(string_acceptor_accepted) {
    std::set<string_impl> a = sa->accepted();
    BOOST_CHECK(a.count("e") > 0);
    BOOST_CHECK(a.count("ei") > 0);
    BOOST_CHECK(a.count("ein") > 0);
    BOOST_CHECK(a.count("eins") > 0);
    BOOST_CHECK(a.count("zeit") == 0);
}

BOOST_AUTO_TEST_CASE(string_acceptor_accepted_vectors) {
    std::set<std::vector<string_impl>> a = sa->accepted_vectors();
    const std::vector<string_impl> v1 {"e", "i", "n", "s"};
    BOOST_CHECK(a.count(v1) > 0);
    const std::vector<string_impl> v2 {"z", "w", "e", "i", "<#>"};
    BOOST_CHECK(a.count(v2) > 0);
    const std::vector<string_impl> v3 {"z", "w", "e", "i", "t", "e", "<#>"};
    BOOST_CHECK(a.count(v3) == 0);
}

BOOST_AUTO_TEST_CASE(add_word) {
    BOOST_CHECK(!sa->accepts("eine"));
    sa->add_word("eine");
    BOOST_CHECK(sa->accepts("eine"));
    const std::vector<string_impl> v3 {"z", "w", "e", "i", "t", "e", "<#>"};
    BOOST_CHECK(!sa->accepts(v3));
    sa->add_word(v3);
    BOOST_CHECK(sa->accepts(v3));
}

BOOST_AUTO_TEST_CASE(add_word_partials) {
    BOOST_REQUIRE(!sa->accepts("zeit"));
    sa->add_word("zeit");
    BOOST_CHECK(sa->accepts("zeit"));
    BOOST_CHECK(!sa->accepts("zei"));
    sa->add_word("zeit", true);
    BOOST_CHECK(sa->accepts("zeit"));
    BOOST_CHECK(sa->accepts("zei"));
    // word containing unknown symbols:
    sa->add_word("käsekuchen", true);
    BOOST_CHECK(sa->accepts("käsekuchen"));
    BOOST_CHECK(sa->accepts("käseku"));
}

BOOST_AUTO_TEST_SUITE_END()

//////////////////////// Transducer //////////////////////

struct GfsmTransducerFixture {
    Transducer* fsm;
    Gfsm::AutomatonBuilder& _builder = Gfsm::AutomatonBuilder::instance();
    const LabelVector v {4}, n {5}, d {6}, u {8}, s {9}, m {10};
    const LabelVector vn {4, 5}, un {8, 5}, um {8, 10},
                      mu {10, 8}, nd {5, 6};
    const LabelVector vnd {4, 5, 6},
                      vns {4, 5, 9},
                      und {8, 5, 6},
                      uns {8, 5, 9},
                      vvv {4, 4, 4},
                      uuu {8, 8, 8};
    const LabelVector mund {10, 8, 5, 6};

    GfsmTransducerFixture() {
        fsm = new Transducer(_builder.make_transducer());
    }
    ~GfsmTransducerFixture() {
        delete fsm;
    }
};

BOOST_FIXTURE_TEST_SUITE(Transducer1, GfsmTransducerFixture)

BOOST_AUTO_TEST_CASE(empty_transducer) {
    std::set<Path> results = fsm->transduce(vnd);
    BOOST_CHECK(results.empty());
}

BOOST_AUTO_TEST_CASE(transducer_add_path) {
    fsm->add_path(Path(v, u, 0.5));
    std::set<Path> results = fsm->transduce(v);
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto it = results.begin();
    BOOST_CHECK((*it).input  == v);
    BOOST_CHECK((*it).output == u);
    BOOST_CHECK_CLOSE((*it).weight, 0.5, 0.0001);
    results = fsm->transduce(vvv);
    BOOST_CHECK_EQUAL(results.size(), 0);
}

BOOST_AUTO_TEST_CASE(transducer_add_cyclic_path) {
    fsm->add_cyclic_path(Path(v, u, 0.5));
    std::set<Path> results = fsm->transduce(v);
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto it = results.begin();
    BOOST_CHECK((*it).input  == v);
    BOOST_CHECK((*it).output == u);
    BOOST_CHECK_CLOSE((*it).weight, 0.5, 0.0001);
    results = fsm->transduce(vvv);
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto iu = results.begin();
    BOOST_CHECK((*iu).input  == vvv);
    BOOST_CHECK((*iu).output == uuu);
    BOOST_CHECK_CLOSE((*iu).weight, 1.5, 0.0001);
}

BOOST_AUTO_TEST_CASE(transducer_add_cyclic_paths_2) {
    fsm->add_cyclic_path(Path(v, u, 0.5));
    fsm->add_cyclic_path(Path(n, n, 0.0));
    fsm->add_cyclic_path(Path(d, d, 0.0));
    std::set<Path> results = fsm->transduce(vnd);
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto it = results.begin();
    BOOST_CHECK((*it).input  == vnd);
    BOOST_CHECK((*it).output == und);
    BOOST_CHECK_CLOSE((*it).weight, 0.5, 0.0001);
}

BOOST_AUTO_TEST_CASE(transducer_add_cyclic_paths_3) {
    fsm->add_cyclic_path(Path(vn, un, 0.5));
    fsm->add_cyclic_path(Path(d, d, 0.0));
    std::set<Path> results = fsm->transduce(vnd);
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto it = results.begin();
    BOOST_CHECK((*it).input  == vnd);
    BOOST_CHECK((*it).output == und);
    BOOST_CHECK_CLOSE((*it).weight, 0.5, 0.0001);
}

BOOST_AUTO_TEST_CASE(transducer_add_cyclic_paths_4) {
    fsm->add_cyclic_path(Path(v, u, 0.5));
    fsm->add_cyclic_path(Path(nd, m, 1.4));
    std::set<Path> results = fsm->transduce(vnd);
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto it = results.begin();
    BOOST_CHECK((*it).input  == vnd);
    BOOST_CHECK((*it).output == um);
    BOOST_CHECK_CLOSE((*it).weight, 1.9, 0.0001);
}

BOOST_AUTO_TEST_CASE(transducer_add_cyclic_paths_5) {
    fsm->add_cyclic_path(Path(v, mu, 0.75));
    fsm->add_cyclic_path(Path(n, n, 0.0));
    fsm->add_cyclic_path(Path(d, d, 0.0));
    std::set<Path> results = fsm->transduce(vnd);
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto it = results.begin();
    BOOST_CHECK((*it).input  == vnd);
    BOOST_CHECK((*it).output == mund);
    BOOST_CHECK_CLOSE((*it).weight, 0.75, 0.0001);
}

BOOST_AUTO_TEST_CASE(transducer_add_cyclic_paths_deletion) {
    fsm->add_cyclic_path(Path(v, u,  0.75));
    fsm->add_cyclic_path(Path(n, n,  0.0));
    fsm->add_cyclic_path(Path(d, {}, 0.33));
    std::set<Path> results = fsm->transduce(vnd);
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto it = results.begin();
    BOOST_CHECK((*it).input  == vnd);
    BOOST_CHECK((*it).output == un);
    BOOST_CHECK_CLOSE((*it).weight, 1.08, 0.0001);
}

BOOST_AUTO_TEST_CASE(transducer_add_cyclic_paths_multiple) {
    fsm->add_cyclic_path(Path(v, v, 0.0));
    fsm->add_cyclic_path(Path(n, n, 0.0));
    fsm->add_cyclic_path(Path(d, d, 0.0));
    fsm->add_cyclic_path(Path(d, s, 0.1));
    fsm->add_cyclic_path(Path(v, u, 0.5));
    std::set<Path> results = fsm->transduce(vnd);
    std::map<LabelVector, double> expected
           {std::make_pair(vnd, 0.0),
            std::make_pair(vns, 0.1),
            std::make_pair(und, 0.5),
            std::make_pair(uns, 0.6)};
    BOOST_REQUIRE_EQUAL(results.size(), expected.size());
    for (const Path& p : results) {
        BOOST_CHECK(p.input == vnd);
        BOOST_REQUIRE(expected.count(p.output) > 0);
        BOOST_CHECK_CLOSE(expected.at(p.output), p.weight, 0.0001);
        expected.erase(p.output);
    }
    BOOST_CHECK_EQUAL(expected.size(), 0);
}

BOOST_AUTO_TEST_CASE(transducer_add_nonfinal) {
    fsm->add_path(Path(v, u, 0.5), false, false);
    std::set<Path> results = fsm->transduce(v);
    BOOST_CHECK_EQUAL(results.size(), 0);
    fsm->add_cyclic_path(Path(v, n, 0.2), false);
    results = fsm->transduce(v);
    BOOST_CHECK_EQUAL(results.size(), 0);
    fsm->add_cyclic_path(Path(v, mu, 0.33), true);
    results = fsm->transduce(v);
    std::map<LabelVector, double> expected
           {std::make_pair(n, 0.2),
            std::make_pair(mu, 0.33)};
    BOOST_CHECK_EQUAL(results.size(), expected.size());
    for (const Path& p : results) {
        BOOST_CHECK(p.input == v);
        BOOST_REQUIRE(expected.count(p.output) > 0);
        BOOST_CHECK_CLOSE(expected.at(p.output), p.weight, 0.0001);
        expected.erase(p.output);
    }
    BOOST_CHECK_EQUAL(expected.size(), 0);
}

BOOST_AUTO_TEST_CASE(transducer_add_nonfinal_2) {
    const LabelVector eps {};
    fsm->add_cyclic_path(Path(v, un, 0.5), false);
    fsm->add_path(Path(eps, d, 0.2));
    std::set<Path> results = fsm->transduce(v);
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto it = results.begin();
    BOOST_CHECK((*it).input  == v);
    BOOST_CHECK((*it).output == und);
    BOOST_CHECK_CLOSE((*it).weight, 0.7, 0.0001);
    results = fsm->transduce(eps);
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto iu = results.begin();
    BOOST_CHECK((*iu).input  == eps);
    BOOST_CHECK((*iu).output == d);
    BOOST_CHECK_CLOSE((*iu).weight, 0.2, 0.0001);
}

BOOST_AUTO_TEST_CASE(transducer_accepted_paths) {
    fsm->add_path(Path(v, mu, 0.5));
    std::set<Path> accepted = fsm->accepted_paths(true);
    BOOST_REQUIRE_EQUAL(accepted.size(), 1);
    auto it = accepted.begin();
    BOOST_CHECK((*it).input  == v);
    BOOST_CHECK((*it).output == mu);
    BOOST_CHECK_CLOSE((*it).weight, 0.5, 0.0001);
}

BOOST_AUTO_TEST_CASE(transducer_accepted_paths_cyclic) {
    fsm->add_cyclic_path(Path(v, mu, 0.5));
    std::set<Path> accepted = fsm->accepted_paths();
    // automaton is cyclic, so accepted_paths() should return empty set
    // as it would run into infinite recursion otherwise
    BOOST_CHECK_EQUAL(accepted.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()

///////////////////// StringTransducer ///////////////////

struct GfsmStringTransducerFixture {
    StringTransducer* fsm;
    Alphabet in, out;
    Gfsm::AutomatonBuilder& _builder = Gfsm::AutomatonBuilder::instance();
    const std::vector<string_impl> vnd {"v", "n", "d"},
        und {"u", "n", "d"}, um {"u", "m"}, mund {"m", "u", "n", "d"},
        vns {"v", "n", "s"}, uns {"u", "n", "s"};

    GfsmStringTransducerFixture() {
        fsm = new StringTransducer(_builder.make_stringtransducer());
        in.cover("abcdefghijklmnopqrstuvwxyzäöü");
        out.cover("dmnsuv");
        fsm->set_input_alphabet(in);
        fsm->set_output_alphabet(out);
    }
    ~GfsmStringTransducerFixture() {
        delete fsm;
    }
};

BOOST_FIXTURE_TEST_SUITE(StringTransducer1, GfsmStringTransducerFixture)

BOOST_AUTO_TEST_CASE(string_transducer_add_paths) {
    fsm->add_cyclic_path("v", "u", 0.5);
    fsm->add_cyclic_path("n", "n", 0.0);
    fsm->add_cyclic_path("d", "d", 0.0);
    std::set<StringPath> results = fsm->transduce("vnd");
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto it = results.begin();
    BOOST_CHECK((*it).input  == vnd);
    BOOST_CHECK((*it).output == und);
    BOOST_CHECK_CLOSE((*it).weight, 0.5, 0.0001);
}

BOOST_AUTO_TEST_CASE(string_transducer_add_paths_2) {
    const std::vector<string_impl> v {"v"}, u {"u"}, n {"n"}, d {"d"};
    fsm->add_cyclic_path(StringPath(v, u, 0.5));
    fsm->add_cyclic_path(StringPath(n, n, 0.0));
    fsm->add_cyclic_path(StringPath(d, d, 0.0));
    std::set<StringPath> results = fsm->transduce(vnd);
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto it = results.begin();
    BOOST_CHECK((*it).input  == vnd);
    BOOST_CHECK((*it).output == und);
    BOOST_CHECK_CLOSE((*it).weight, 0.5, 0.0001);
}

BOOST_AUTO_TEST_CASE(string_transducer_add_paths_3) {
    fsm->add_cyclic_path("vn", "un", 0.5);
    fsm->add_cyclic_path("d", "d", 0.0);
    std::set<StringPath> results = fsm->transduce(vnd);
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto it = results.begin();
    BOOST_CHECK((*it).input  == vnd);
    BOOST_CHECK((*it).output == und);
    BOOST_CHECK_CLOSE((*it).weight, 0.5, 0.0001);
}

BOOST_AUTO_TEST_CASE(string_transducer_add_paths_4) {
    fsm->add_cyclic_path("v", "u", 0.5);
    fsm->add_cyclic_path("nd", "m", 1.4);
    std::set<StringPath> results = fsm->transduce("vnd");
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto it = results.begin();
    BOOST_CHECK((*it).input  == vnd);
    BOOST_CHECK((*it).output == um);
    BOOST_CHECK_CLOSE((*it).weight, 1.9, 0.0001);
}

BOOST_AUTO_TEST_CASE(string_transducer_add_paths_5) {
    const std::vector<string_impl> v {"v"}, n {"n"}, d {"d"},
                                   mu {"m", "u"};
    fsm->add_cyclic_path(StringPath(v, mu, 0.75));
    fsm->add_cyclic_path(StringPath(n, n, 0.0));
    fsm->add_cyclic_path(StringPath(d, d, 0.0));
    std::set<StringPath> results = fsm->transduce(vnd);
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto it = results.begin();
    BOOST_CHECK((*it).input  == vnd);
    BOOST_CHECK((*it).output == mund);
    BOOST_CHECK_CLOSE((*it).weight, 0.75, 0.0001);
}

BOOST_AUTO_TEST_CASE(string_transducer_add_paths_deletion) {
    fsm->add_cyclic_path("v", "u", 0.5);
    fsm->add_cyclic_path("n", "m", 0.7);
    fsm->add_cyclic_path("d", "",  0.7);
    std::set<StringPath> results = fsm->transduce("vnd");
    BOOST_REQUIRE_EQUAL(results.size(), 1);
    auto it = results.begin();
    BOOST_CHECK((*it).input  == vnd);
    BOOST_CHECK((*it).output == um);
    BOOST_CHECK_CLOSE((*it).weight, 1.9, 0.0001);
}

BOOST_AUTO_TEST_CASE(string_transducer_add_paths_multiple) {
    fsm->add_cyclic_path("v", "v", 0.0);
    fsm->add_cyclic_path("n", "n", 0.0);
    fsm->add_cyclic_path("d", "d", 0.0);
    fsm->add_cyclic_path("d", "s", 0.1);
    fsm->add_cyclic_path("v", "u", 0.5);
    std::set<StringPath> results = fsm->transduce(vnd);
    std::set<Path> results_vec = fsm->transduce(in.map_symbols(vnd));
    std::set<Path> results_str;
    for (const StringPath& p : results) {
        results_str.insert(Path(in.map_symbols(p.input),
                                out.map_symbols(p.output),
                                p.weight));
    }
    BOOST_REQUIRE(results_vec == results_str);
    std::map<std::vector<string_impl>, double> expected
           {std::make_pair(vnd, 0.0),
            std::make_pair(vns, 0.1),
            std::make_pair(und, 0.5),
            std::make_pair(uns, 0.6)};
    BOOST_REQUIRE_EQUAL(results.size(), expected.size());
    for (const StringPath& p : results) {
        BOOST_CHECK(p.input == vnd);
        BOOST_REQUIRE(expected.count(p.output) > 0);
        BOOST_CHECK_CLOSE(expected.at(p.output), p.weight, 0.0001);
        expected.erase(p.output);
    }
    BOOST_CHECK_EQUAL(expected.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()

//////////////////////// Cascade /////////////////////////

struct GfsmCascadeFixture {
    Cascade* csc;
    Gfsm::AutomatonBuilder& _builder = Gfsm::AutomatonBuilder::instance();
    const LabelVector vec_in {1, 2}, vec_best {3, 2}, vec_alt {5, 2};
    const LabelVector triple_in {1, 1, 1}, triple_best {4, 4, 4},
        triple_alt {3, 3, 3};

    GfsmCascadeFixture() {
        // members
        Transducer fst = _builder.make_transducer();
        Acceptor   fsa = _builder.make_acceptor();
        fst.add_cyclic_path(Path(LabelVector {1}, LabelVector {4}, 0.05));
        fst.add_cyclic_path(Path(LabelVector {1}, LabelVector {5}, 0.7));
        fst.add_cyclic_path(Path(LabelVector {2}, LabelVector {2}, 0.0));
        fst.add_cyclic_path(Path(LabelVector {2}, LabelVector {4}, 0.1));
        fst.add_cyclic_path(Path(LabelVector {1, 2},
                                 LabelVector {3, 2}, 0.4));
        fst.add_cyclic_path(Path(LabelVector {1}, LabelVector {3}, 0.3));
        fsa.add_path(LabelVector {5, 2});
        fsa.add_path(LabelVector {3, 2});
        fsa.add_path(LabelVector {3, 3, 3});
        fsa.add_path(LabelVector {4, 4, 4});
        // cascade
        csc = new Cascade(_builder.make_cascade(2));
        csc->append(&fst);
        csc->append(&fsa);
        csc->sort();
    }
    ~GfsmCascadeFixture() {
        delete csc;
    }
};

BOOST_FIXTURE_TEST_SUITE(Cascade1, GfsmCascadeFixture)

BOOST_AUTO_TEST_CASE(cascade_defaults) {
    BOOST_CHECK_EQUAL(csc->get_max_paths(),  1);
    BOOST_CHECK(csc->get_max_ops() >= 2147483647);
    BOOST_CHECK_CLOSE(csc->get_max_weight(), 0.0, 0.001);
}

BOOST_AUTO_TEST_CASE(cascade_lookup_best_path) {
    csc->set_max_paths(1);
    csc->set_max_weight(10.0);
    std::set<Path> best = csc->lookup_nbest(vec_in);
    BOOST_REQUIRE_EQUAL(best.size(), 1);
    auto it = best.begin();
    BOOST_CHECK((*it).input  == vec_in);
    BOOST_CHECK((*it).output == vec_best);
    BOOST_CHECK_CLOSE((*it).weight, 0.3, 0.0001);
}

BOOST_AUTO_TEST_CASE(cascade_lookup_best_path_2) {
    csc->set_max_paths(1);
    csc->set_max_weight(10.0);
    std::set<Path> best = csc->lookup_nbest(triple_in);
    BOOST_REQUIRE_EQUAL(best.size(), 1);
    auto it = best.begin();
    BOOST_CHECK((*it).input  == triple_in);
    BOOST_CHECK((*it).output == triple_best);
    BOOST_CHECK_CLOSE((*it).weight, 0.15, 0.0001);
}

BOOST_AUTO_TEST_CASE(cascade_lookup_best_paths) {
    csc->set_max_paths(10);
    csc->set_max_weight(10.0);
    std::set<Path> results = csc->lookup_nbest(vec_in);
    std::map<LabelVector, double> expected
           {std::make_pair(vec_best, 0.3),
            std::make_pair(vec_alt,  0.7)};
    BOOST_REQUIRE_EQUAL(results.size(), expected.size());
    for (const Path& p : results) {
        BOOST_CHECK(p.input == vec_in);
        BOOST_REQUIRE(expected.count(p.output) > 0);
        BOOST_CHECK_CLOSE(expected.at(p.output), p.weight, 0.0001);
        expected.erase(p.output);
    }
    BOOST_CHECK_EQUAL(expected.size(), 0);
}

BOOST_AUTO_TEST_CASE(cascade_lookup_best_paths_2) {
    csc->set_max_paths(10);
    csc->set_max_weight(10.0);
    std::set<Path> results = csc->lookup_nbest(triple_in);
    std::map<LabelVector, double> expected
           {std::make_pair(triple_best, 0.15),
            std::make_pair(triple_alt,  0.9)};
    BOOST_REQUIRE_EQUAL(results.size(), expected.size());
    for (const Path& p : results) {
        BOOST_CHECK(p.input == triple_in);
        BOOST_REQUIRE(expected.count(p.output) > 0);
        BOOST_CHECK_CLOSE(expected.at(p.output), p.weight, 0.0001);
        expected.erase(p.output);
    }
    BOOST_CHECK_EQUAL(expected.size(), 0);
}

BOOST_AUTO_TEST_CASE(cascade_lookup_best_paths_weight_cutoff) {
    csc->set_max_paths(10);
    csc->set_max_weight(0.5);
    std::set<Path> best = csc->lookup_nbest(vec_in);
    BOOST_REQUIRE_EQUAL(best.size(), 1);
    auto it = best.begin();
    BOOST_CHECK((*it).input  == vec_in);
    BOOST_CHECK((*it).output == vec_best);
    BOOST_CHECK_CLOSE((*it).weight, 0.3, 0.0001);
}

BOOST_AUTO_TEST_SUITE_END()

///////////////////// StringCascade //////////////////////

struct GfsmStringCascadeFixture {
    StringCascade* csc;
    Gfsm::AutomatonBuilder& _builder = Gfsm::AutomatonBuilder::instance();
    const std::vector<string_impl> str_jn {"j", "n"}, str_in {"i", "n"},
        str_an {"a", "n"}, str_ahn {"a", "h", "n"}, str_ihn {"i", "h", "n"};

    GfsmStringCascadeFixture() {
        // members
        StringTransducer fst = _builder.make_stringtransducer();
        StringAcceptor   fsa = _builder.make_stringacceptor();
        Alphabet alph_in, alph_out;
        alph_in.cover("njihga");
        alph_out.cover("abcdefghijklmnopqrstuvwxyzäöüß");
        fst.set_input_alphabet(alph_in);
        fst.set_output_alphabet(alph_out);
        fsa.set_alphabet(alph_out);
        // make some paths
        fst.add_cyclic_path(StringPath({"n"}, {"n"}, 0.0));
        fst.add_cyclic_path(StringPath({"n"}, {"h", "n"}, 0.8));
        fst.add_cyclic_path(StringPath({"j"}, {"j"}, 0.0));
        fst.add_cyclic_path(StringPath({"j"}, {"i"}, 0.05));
        fst.add_cyclic_path(StringPath({"j"}, {"a"}, 0.7));
        fst.add_cyclic_path(StringPath({"j"}, {"i", "h"}, 0.3));
        fst.add_cyclic_path(StringPath({"i"}, {"i"}, 0.0));
        fsa.add_word("in");
        fsa.add_word("ihn");
        fsa.add_word("an");
        fsa.add_word("ahn");
        // cascade
        csc = new StringCascade(_builder.make_stringcascade(2));
        csc->append(fst);
        csc->append(fsa);
        csc->sort();
    }
    ~GfsmStringCascadeFixture() {
        delete csc;
    }
};

BOOST_FIXTURE_TEST_SUITE(StringCascade1, GfsmStringCascadeFixture)

BOOST_AUTO_TEST_CASE(stringcascade_defaults) {
    BOOST_CHECK_EQUAL(csc->get_max_paths(),  1);
    BOOST_CHECK(csc->get_max_ops() >= 2147483647);
    BOOST_CHECK_CLOSE(csc->get_max_weight(), 0.0, 0.001);
}

BOOST_AUTO_TEST_CASE(stringcascade_lookup_best_path) {
    csc->set_max_paths(1);
    csc->set_max_weight(10.0);
    std::set<StringPath> best = csc->lookup_nbest(str_in);
    BOOST_REQUIRE_EQUAL(best.size(), 1);
    auto it = best.begin();
    BOOST_CHECK((*it).input  == str_in);
    BOOST_CHECK((*it).output == str_in);
    BOOST_CHECK_CLOSE((*it).weight, 0.0, 0.0001);
}

BOOST_AUTO_TEST_CASE(stringcascade_lookup_best_path_2) {
    csc->set_max_paths(1);
    csc->set_max_weight(10.0);
    std::set<StringPath> best = csc->lookup_nbest(str_jn);
    BOOST_REQUIRE_EQUAL(best.size(), 1);
    auto it = best.begin();
    BOOST_CHECK((*it).input  == str_jn);
    BOOST_CHECK((*it).output == str_in);
    BOOST_CHECK_CLOSE((*it).weight, 0.05, 0.0001);
}

BOOST_AUTO_TEST_CASE(cascade_lookup_best_paths) {
    csc->set_max_paths(10);
    csc->set_max_weight(10.0);
    std::set<StringPath> results = csc->lookup_nbest(str_jn);
    std::map<std::vector<string_impl>, double> expected
           {std::make_pair(str_in,  0.05),
            std::make_pair(str_ihn, 0.3),
            std::make_pair(str_an,  0.7),
            std::make_pair(str_ahn, 1.5)};
    BOOST_REQUIRE_EQUAL(results.size(), expected.size());
    for (const StringPath& p : results) {
        BOOST_CHECK(p.input == str_jn);
        BOOST_REQUIRE(expected.count(p.output) > 0);
        BOOST_CHECK_CLOSE(expected.at(p.output), p.weight, 0.0001);
        expected.erase(p.output);
    }
    BOOST_CHECK_EQUAL(expected.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()
