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

from norma import Lexicon as CppLexicon

# Extends the C++ bindings for Lexicon by providing some convenience
# functions that are much simpler to implement on the Python side
class Lexicon(CppLexicon):
    """A lexicon holding a set of wordforms.

    Lexicon objects are used by some normalizers to restrict the
    output of normalization candidates to words contained within the
    lexicon.

    Supports only a small subset of Python's container interface,
    namely:
      * len(lexicon)
      * Membership tests (if word in lexicon: ...)
      * Iterators (for word in lexicon: ...)

    If you need more of Python's built-in functions for containers,
    make a copy of the Lexicon's entries and work with that:
      x = set(lexicon.entries)
    or
      x = [w for w in lexicon.entries]

    """

    def __init__(self, lexfile=None, symfile=None):
        """Construct and initialize the lexicon.

        Keyword arguments:
          lexfile -- Name of the lexicon file
          symfile -- Name of the symbols file
        """
        super(Lexicon, self).__init__()
        if lexfile is not None:
            self.lexfile = lexfile
        if symfile is not None:
            self.symfile = symfile
        self.init()

    def __iter__(self):
        return self.entries.__iter__()

    def add(self, *args):
        """Add new lexicon entries.

        Adds all supplied arguments to the lexicon.
        """
        for word in args:
            super(Lexicon, self).add(word)

    def extend(self, args):
        """Extend the lexicon with a list of entries.

        extend(['foo', 'bar']) is equivalent to add('foo', 'bar').
        """
        self.add(*args)
