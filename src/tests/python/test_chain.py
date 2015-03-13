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

import norma.NormalizerWrapper as Normalizer

class ChainTest(unittest.TestCase, AssertFloat):
    rulesfile = TEST_BASE_DIR + "/test-rulesfile.txt"
    mapfile = TEST_BASE_DIR + "/test-mapfile.txt"
    wldfile = TEST_BASE_DIR + "/test-weights.txt"

    def setUp(self):
        self.lex  = make_test_lexicon()
        self.lex.add("in")
        self.lex.add("ihm")
        self.norm = Normalizer.Chain()

    def testNormalize(self):
        norm1 = Normalizer.Rulebased(self.rulesfile, self.lex)
        norm2 = Normalizer.Mapper(self.mapfile, self.lex)
        norm3 = Normalizer.WLD(self.wldfile, self.lex)
        self.norm = Normalizer.Chain(norm1, norm2, norm3)
        self.norm.threshold = 0.001
        self.assertClose(self.norm("vnd"),  ("eins", 0.011, "RuleBased"))
        self.assertClose(self.norm("jn"),   ("in",   0.75,  "Mapper"))
        self.assertClose(self.norm("jm"),   ("ihm",  0.449, "WLD"))
        self.assertClose(self.norm("foo"),  ("in",   0.050, "WLD"))
        # it's really hard to make WLD fail...
        self.assertClose(self.norm("mississippi"), ("mississippi", 0, "[None]"))

    def testNormalizeNBest(self):
        norm1 = Normalizer.Mapper(self.mapfile, self.lex)
        norm2 = Normalizer.Rulebased(self.rulesfile, self.lex)
        norm3 = Normalizer.WLD(self.wldfile, self.lex)
        self.norm = Normalizer.Chain(norm1, norm2, norm3)
        actual   = self.norm("vnd", 10)
        expected = [("und", 1.0, "Mapper"),
                    ("in",  0.135, "WLD"),
                    ("ihm", 0.050, "WLD"),
                    ("eins", 0.011, "RuleBased")]
        self.assertEquals(len(actual), len(expected))
        for (x, y) in zip(actual, expected):
            self.assertClose(x, y)
        
    def testTrain(self):
        norm1 = Normalizer.Mapper()
        norm2 = Normalizer.Rulebased()
        norm3 = Normalizer.WLD()
        self.norm = Normalizer.Chain(norm1, norm2, norm3)
        self.norm.lexicon = self.lex
        self.assertEquals(self.norm("zwey"),     ("zwey",     0, "[None]"))
        self.assertEquals(self.norm("zweytens"), ("zweytens", 0, "[None]"))
        self.assertEquals(self.norm("drey"),     ("drey",     0, "[None]"))
        # train
        data = [("zwey", "zwei"), ("zweyte", "zweite"), ("zweitens", "zweitens")]
        self.norm.train(data)
        norm3.perform_training()  # hack :(
        # test
        self.assertClose(self.norm("zwey"),     ("zwei",     1.0,   "Mapper"))
        self.assertClose(self.norm("zweytens"), ("zweitens", 0.708, "RuleBased"))
        self.assertClose(self.norm("drey"),     ("drei",     1.0,   "WLD"))

    def testList(self):
        norm1 = Normalizer.Mapper()
        norm2 = Normalizer.Rulebased()
        norm3 = Normalizer.WLD()
        self.norm = Normalizer.Chain(norm1, norm2, norm3)
        self.assertEquals(len(self.norm), 3)
        names = [x.name for x in self.norm]
        self.assertEquals(names, ['Mapper', 'RuleBased', 'WLD'])
        self.norm.reverse()
        names = [x.name for x in self.norm]
        self.assertEquals(names, ['WLD', 'RuleBased', 'Mapper'])
        self.norm.append(norm2)
        names = [x.name for x in self.norm]
        self.assertEquals(names, ['WLD', 'RuleBased', 'Mapper', 'RuleBased'])
        self.norm.clear()
        self.assertEquals(len(self.norm), 0)
