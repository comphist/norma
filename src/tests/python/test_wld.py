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

def make_wld_lexicon():
    lex = Lexicon()
    lex.init()
    lex.extend(["an", "ja", "in", "ihn", "ihm", "und"])
    return lex

class WLDTest(unittest.TestCase, AssertFloat):
    paramfile = TEST_BASE_DIR + "/test-weights.txt"

    def setUp(self):
        self.norm = Normalizer.WLD()

    def testName(self):
        self.assertEquals(self.norm.name, "WLD")

    def testProperties1(self):
        self.assertEquals(self.norm.paramfile, '')
        self.norm.paramfile = self.paramfile
        self.assertEquals(self.norm.paramfile, self.paramfile)

    def testProperties2(self):
        self.assertTrue(self.norm.caching)
        self.norm.caching = False
        self.assertFalse(self.norm.caching)

    def testProperties3(self):
        self.norm.ngrams  = 2
        self.norm.divisor = 10
        self.assertEquals(self.norm.ngrams, 2)
        self.assertEquals(self.norm.divisor, 10)

    def testNormalize1(self):
        self.norm.paramfile = self.paramfile
        self.norm.lexicon = make_wld_lexicon()
        self.norm.init()
        self.assertClose(self.norm("in"),  ("in",  1.0,   "WLD"))
        self.assertClose(self.norm("jn"),  ("in",  0.818, "WLD"))
        self.assertClose(self.norm("jhn"), ("ihn", 0.818, "WLD"))
        self.assertClose(self.norm("jm"),  ("ihm", 0.449, "WLD"))

    def testNormalize2(self):
        self.norm.paramfile = self.paramfile
        self.norm.lexicon = make_wld_lexicon()
        self.norm.init()
        rslist = self.norm("jn", 5)
        self.assertEquals(len(rslist), 4)
        self.assertClose(rslist[0], ("in",  0.818, "WLD"))
        self.assertClose(rslist[1], ("ihn", 0.449, "WLD"))
        self.assertClose(rslist[2], ("an",  0.368, "WLD"))
        self.assertClose(rslist[3], ("ihm", 0.183, "WLD"))

    def testTrain(self):
        self.norm.lexicon = make_test_lexicon()
        self.norm.init()
        self.norm.train([('zwey', 'zwei'), ('foo', 'bar')])
        self.norm.perform_training()  # hack :(
        self.assertClose(self.norm('zwey'),     ("zwei",   1.0,   "WLD"))
        self.assertClose(self.norm('drey'),     ("drei",   1.0,   "WLD"))
        self.assertClose(self.norm('foo'),      ("foo",    0.0,   "WLD"))
        self.assertClose(self.norm('siebtens'), ("sieben", 0.135, "WLD"))
        self.assertClose(self.norm('neyn'),     ("nein",   1.0,   "WLD"))

    def testInit2(self):
        self.norm = Normalizer.WLD()
        self.norm.init(self.paramfile, make_wld_lexicon())
        self.assertClose(self.norm("jn"), ("in", 0.818, "WLD"))

    def testInit3(self):
        self.norm = Normalizer.WLD()
        self.norm.lexicon = make_wld_lexicon()
        self.norm.init(self.paramfile)
        self.assertClose(self.norm("jn"), ("in", 0.818, "WLD"))

    def testInitFactory(self):
        self.norm = Normalizer.WLD(self.paramfile, make_wld_lexicon())
        self.assertClose(self.norm("jn"), ("in", 0.818, "WLD"))

    def testNonExistantFilename(self):
        with self.assertRaises(NormaInitError):
            self.norm.init("<dummy>", make_wld_lexicon())
