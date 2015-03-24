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

class ResultTest(unittest.TestCase, AssertFloat):
    def setUp(self):
        self.norm = Normalizer.Mapper()

    def testIdenticalResultsWithFloat(self):
        self.norm.init()
        result_cpp = self.norm("vrouwe")
        result_tuple = ("vrouwe", 0.0, "Mapper")
        result_py = Result("vrouwe", 0.0, "Mapper")
        self.assertEquals(result_cpp, result_tuple)
        self.assertEquals(result_cpp, result_py)
        self.assertEquals(result_py, result_tuple)

    def testIdenticalResultsWithInt(self):
        self.norm.init()
        result_cpp = self.norm("vrouwe")
        result_tuple = ("vrouwe", 0, "Mapper")
        result_py = Result("vrouwe", 0, "Mapper")
        self.assertEquals(result_cpp, result_tuple)
        self.assertEquals(result_cpp, result_py)
        self.assertEquals(result_py, result_tuple)
