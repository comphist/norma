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

def make_test_lexicon():
    lex = Lexicon()
    lex.lexfile = test_vars['lexfile']
    lex.symfile = test_vars['symfile']
    lex.init()
    return lex

class LexiconTest(unittest.TestCase):
    def setUp(self):
        self.lex = Lexicon()
        self.lex.init()

    def testProperties(self):
        self.assertEqual(self.lex.lexfile, '')
        self.assertEqual(self.lex.symfile, '')
        self.lex.lexfile = "foo.lex"
        self.assertEqual(self.lex.lexfile, 'foo.lex')
        self.lex.symfile = "foo.sym"
        self.assertEqual(self.lex.symfile, 'foo.sym')

    def testStaticSymbols(self):
        self.assertEqual(self.lex.symbol_boundary, '<#>')
        self.assertEqual(self.lex.symbol_any, '<any>')
        self.assertEqual(self.lex.symbol_epsilon, '<eps>')

    def testNonExistantFilenames(self):
        self.lex.lexfile = "foo.lex"
        with self.assertRaises(RuntimeError):
            self.lex.init()
        self.lex.symfile = "foo.sym"
        with self.assertRaises(NormaInitError):
            self.lex.init()

    def testAddContains(self):
        self.assertFalse(self.lex.contains("foo"))
        self.lex.add("foo")
        self.assertTrue(self.lex.contains("foo"))
        self.assertFalse(self.lex.contains("bar"))
        self.assertFalse(self.lex.contains("fo"))
        self.lex.add("bar")
        self.assertTrue(self.lex.contains("bar"))

    def testAddContainsPartial(self):
        self.assertFalse(self.lex.contains_partial("foo"))
        self.assertFalse(self.lex.contains_partial("f"))
        self.lex.add("foo")
        self.assertTrue(self.lex.contains_partial("foo"))
        self.assertTrue(self.lex.contains_partial("f"))
        self.assertFalse(self.lex.contains_partial("b"))

    def testInOperator(self):
        self.assertFalse(self.lex.contains("foo"))
        self.lex.add("foo")
        self.assertTrue("foo" in self.lex)

    def testEntries(self):
        self.assertEquals(len(self.lex.entries), 0)
        self.lex.add("foo")
        self.lex.add("bar")
        entries = self.lex.entries
        self.assertEquals(len(self.lex.entries), 2)
        self.assertEquals(len(self.lex), 2)
        for entry in entries:
            self.assertTrue(entry in ("foo", "bar"))

    def testLoadFromFile(self):
        self.lex.lexfile = test_vars['lexfile']
        self.lex.symfile = test_vars['symfile']
        self.lex.init()
        self.assertTrue("eins" in self.lex)
        self.assertTrue("zwei" in self.lex)
        self.assertTrue(u"eins" in self.lex)
        self.assertTrue(u"zwei" in self.lex)
        self.assertTrue(self.lex.contains_partial("sieb"))
        self.assertTrue(self.lex.contains_partial(u"sieb"))
        self.assertEquals(len(self.lex.entries), 12)
        self.assertEquals(len(self.lex), 12)

    def testInitOverload(self):
        self.lex.init(test_vars['lexfile'], test_vars['symfile'])
        self.assertTrue("eins" in self.lex)
        self.assertTrue(self.lex.contains_partial("sieb"))
        self.assertEquals(len(self.lex), 12)

    def testCtorOverload(self):
        self.lex = Lexicon(test_vars['lexfile'], test_vars['symfile'])
        self.assertTrue("eins" in self.lex)
        self.assertTrue(self.lex.contains_partial("sieb"))
        self.assertEquals(len(self.lex), 12)

    def testExtend(self):
        self.assertFalse("eins" in self.lex)
        self.assertFalse("zwei" in self.lex)
        self.assertFalse("drei" in self.lex)
        self.lex.add("eins", "zwei", "drei")
        self.assertTrue("eins" in self.lex)
        self.assertTrue("zwei" in self.lex)
        self.assertTrue("drei" in self.lex)

    def testIter(self):
        data = ["eins", "zwei", "drei"]
        i = 0
        self.lex.add(*data)
        for entry in self.lex:
            i += 1
            self.assertTrue(entry in data)
        self.assertEquals(i, len(self.lex))

    def testConversion(self):
        self.lex.add("eins", "zwei", "drei")
        x = set(self.lex.entries)
        self.assertEquals(x, set(["eins", "zwei", "drei"]))
