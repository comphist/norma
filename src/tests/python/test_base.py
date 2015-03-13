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

# Tests base normalizer functions (i.e., functions that all
# normalizers share), but instantiates MapperNormalizer for this
# purpose, because 1) we don't expose the Base normalizer class to
# Python, and 2) Mapper is the simplest normalizer available.
class BaseTest(unittest.TestCase):

    def setUp(self):
        self.norm = Normalizer.Mapper()

    def testLexicon(self):
        lex1 = make_test_lexicon()
        self.norm.lexicon = lex1
        lex2 = self.norm.lexicon
        self.assertEquals(len(lex1.entries), len(lex2.entries))
        self.assertEquals("eins" in lex1, "eins" in lex2)
        lex1.add("fünfzehn")
        self.assertTrue("fünfzehn" in lex2)
