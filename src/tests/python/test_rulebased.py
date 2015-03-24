#!python
# -*- encoding: utf-8 -*-
################################################################################
# Copyright 2013-2015 Marcel Bollmann, Florian Petran
#
# This file is part of Norma.
#
# Norma is free software: you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# Norma is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License along
# with Norma.  If not, see <http://www.gnu.org/licenses/>.
################################################################################

class RulebasedTest(unittest.TestCase, AssertFloat):
    rulesfile = TEST_BASE_DIR + "/test-rulesfile.txt"

    def setUp(self):
        self.norm = Normalizer.Rulebased()

    def testName(self):
        self.assertEquals(self.norm.name, "RuleBased")

    def testProperties1(self):
        self.assertEquals(self.norm.rulesfile, '')
        self.norm.rulesfile = self.rulesfile
        self.assertEquals(self.norm.rulesfile, self.rulesfile)

    def testProperties2(self):
        self.assertTrue(self.norm.caching)
        self.norm.caching = False
        self.assertFalse(self.norm.caching)

    def testNormalize1(self):
        self.norm.rulesfile = self.rulesfile
        self.norm.lexicon = make_test_lexicon()
        self.norm.init()
        result = self.norm.normalize("vnd")
        self.assertClose(result, ("eins", 0.0105, "RuleBased"))

    def testLogMessages1(self):
        self.norm.rulesfile = self.rulesfile
        self.norm.lexicon = make_test_lexicon()
        self.norm.init()
        result = self.norm.normalize("vnd")
        self.assertEquals(len(result.messages), 7)
        self.assertEquals(result.messages[0][2], "applied rule: {E->E/#_v}")
        self.assertEquals(result.messages[1][2], "applied rule: {v->ei/#_n}")
        self.assertEquals(result.messages[2][2], "applied rule: {E->E/i_n}")
        self.assertEquals(result.messages[3][2], "applied rule: {n->n/i_d}")
        self.assertEquals(result.messages[4][2], "applied rule: {E->E/n_d}")
        self.assertEquals(result.messages[5][2], "applied rule: {d->s/n_#}")
        self.assertEquals(result.messages[6][2], "applied rule: {E->E/s_#}")

    def testNormalize2(self):
        self.norm.rulesfile = self.rulesfile
        lex = make_test_lexicon()
        lex.add("und")
        self.norm.lexicon = lex
        self.norm.init()
        self.assertClose(self.norm.normalize("vnd"), ("und", 0.0445, "RuleBased"))
        results = self.norm("vnd", 5)
        self.assertEquals(len(results), 2)
        self.assertClose(results[0], ("und",  0.0445, "RuleBased"))
        self.assertClose(results[1], ("eins", 0.0105, "RuleBased"))

    def testNormalize3(self):
        self.norm.init(self.rulesfile, make_test_lexicon())
        fvo = self.norm.normalize("fvo")
        self.assertEquals(fvo, ("fvo", 0.0, "RuleBased"))
        self.assertEquals(len(fvo.messages), 1)
        self.assertEquals(fvo.messages[0][2], "no candidate found")

    def testTrain(self):
        self.norm.lexicon = make_test_lexicon()
        data = [("zwey", "zwei"), ("drei", "drei"), ("drey", "drey")]
        self.norm.train(data)
        self.assertClose(self.norm("zwey"), ("zwei", 0.5625, "RuleBased"))
        self.assertClose(self.norm("drei"), ("drei", 0.75,   "RuleBased"))
        self.assertClose(self.norm("drey"), ("drei", 0.8181, "RuleBased"))

    def testNonExistantFilename(self):
        with self.assertRaises(NormaInitError):
            self.norm.init("<dummy>", make_test_lexicon())
