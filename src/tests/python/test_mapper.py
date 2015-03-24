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

class MapperTest(unittest.TestCase, AssertFloat):
    mapfile = TEST_BASE_DIR + "/test-mapfile.txt"

    def setUp(self):
        self.norm = Normalizer.Mapper()

    def testName(self):
        self.assertEquals(self.norm.name, "Mapper")

    def testProperties(self):
        self.assertEquals(self.norm.mapfile, '')
        self.norm.mapfile = self.mapfile
        self.assertEquals(self.norm.mapfile, self.mapfile)

    def testNormalize(self):
        self.norm.mapfile = self.mapfile
        self.norm.init()
        self.assertEquals(self.norm.normalize("vnd"), ("und", 1.0, "Mapper"))
        self.assertEquals(self.norm.normalize("foo"), ("foo", 0.0, "Mapper"))
        self.assertEquals(self.norm.normalize("jn"),  ("in",  0.75, "Mapper"))
        self.assertEquals(self.norm("vnd"),           ("und", 1.0, "Mapper"))

    def testNormalizeNbest(self):
        self.norm.mapfile = self.mapfile
        self.norm.init()
        self.assertEquals(self.norm.normalize("jn", 1), [("in",  0.75, "Mapper")])
        self.assertEquals(self.norm("jn", 1),           [("in",  0.75, "Mapper")])
        r1 = [("in", 0.75, "Mapper"), ("ihn", 0.20, "Mapper"), ("inne", 0.05, "Mapper")]
        self.assertEquals(self.norm.normalize("jn", 3), r1)
        self.assertEquals(self.norm.normalize("jn", 10), r1)

    def testLogMessageForSuccess(self):
        self.norm.mapfile = self.mapfile
        self.norm.init()
        result = self.norm("vnd")
        self.assertEquals(result, ("und", 1.0, "Mapper"))
        log = result.messages
        self.assertEquals(len(log), 1)
        self.assertEquals(len(log[0]), 3)
        (level, origin, message) = log[0]
        self.assertEquals(level, "TRACE")
        self.assertEquals(origin, "Mapper")
        self.assertEquals(message, "absolute count: 25")

    def testLogMessageForFailure(self):
        self.norm.mapfile = self.mapfile
        self.norm.init()
        result = self.norm("foo")
        self.assertEquals(result, ("foo", 0.0, "Mapper"))
        log = result.messages
        self.assertEquals(len(log), 1)
        self.assertEquals(len(log[0]), 3)
        (level, origin, message) = log[0]
        self.assertEquals(level, "TRACE")
        self.assertEquals(origin, "Mapper")
        self.assertEquals(message, "word not found")

    def testTrain(self):
        self.norm.init()
        self.assertEquals(self.norm("vrouwe"), ("vrouwe", 0.0, "Mapper"))
        data = [("vrouwe", "frau"), ("vrouwe", "frauen"),
                ("vrouwe", "frau"), ("vrouwe", "frau")]
        self.norm.train(data)
        self.assertEquals(self.norm("vrouwe"), ("frau", 0.75, "Mapper"))

    def testTrain2(self):
        self.norm.init()
        self.assertEquals(self.norm("vrouwe"), ("vrouwe", 0.0, "Mapper"))
        self.norm.train("vrouwe", "frau", 3)
        self.norm.train("vrouwe", "frauen", 1)
        self.assertEquals(self.norm("vrouwe"), ("frau", 0.75, "Mapper"))

    def testTrain3(self):
        self.norm.init()
        self.assertEquals(self.norm("zwey"), ("zwey", 0.0, "Mapper"))
        data = [("zwey", "zwei"), ("zweyte", "zweite"), ("zweitens", "zweitens")]
        self.norm.train(data)
        self.assertEquals(self.norm("zwey"), ("zwei", 1.0, "Mapper"))

    def testNonExistantFilename(self):
        with self.assertRaises(NormaInitError):
            self.norm.init("<dummy>")
