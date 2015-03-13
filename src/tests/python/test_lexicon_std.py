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

test_vars['lexfile'] = TEST_BASE_DIR+"/test-lexicon.gfsa"
test_vars['symfile'] = TEST_BASE_DIR+"/test-lexicon.lab"

class LexiconSTDTest(unittest.TestCase):
    def setUp(self):
        self.lex = Lexicon()
        self.lex.init()

    def testEntries1(self):
        self.assertEquals(len(self.lex.entries), 0)
        self.lex.add("fünf")
        self.assertTrue("fünf" in self.lex)

    def testEntries2(self):
        self.assertEquals(len(self.lex.entries), 0)
        self.lex.add("fünf")
        # not the same with STD strings:
        self.assertFalse(u'fünf' in self.lex)

    def testEntries3(self):
        self.assertEquals(len(self.lex.entries), 0)
        self.lex.add(u'fünf')
        # not the same with STD strings:
        self.assertFalse("fünf" in self.lex)

    def testEntries4(self):
        self.assertEquals(len(self.lex.entries), 0)
        self.lex.add(u"fünf")
        self.assertTrue(u"fünf" in self.lex)

    def testEntries5(self):
        self.assertEquals(len(self.lex.entries), 0)
        self.lex.add(u'fünf')
        # implicit latin-1 conversion of unicode:
        self.assertTrue(u'fünf'.encode("latin-1") in self.lex)

    def testUnicode(self):
        # unicode strings with characters outside of latin-1 raise an
        # exception
        with self.assertRaises(UnicodeEncodeError):
            test = (u'\u0409' in self.lex)

    def testUnicode2(self):
        self.assertFalse(u'\u0409'.encode("utf-8") in self.lex)

    def testLoadFromFile(self):
        self.lex.lexfile = test_vars['lexfile']
        self.lex.symfile = test_vars['symfile']
        self.lex.init()
        # doesn't work anymore with STD strings:
        self.assertFalse("fünf" in self.lex)
        self.assertFalse(u'fünf' in self.lex)
        self.assertFalse(self.lex.contains_partial("fü"))

    def testToPythonConversion(self):
        self.lex.lexfile = test_vars['lexfile']
        self.lex.symfile = test_vars['symfile']
        self.lex.init()
        for entry in self.lex.entries:
            self.assertTrue(isinstance(entry, str))
