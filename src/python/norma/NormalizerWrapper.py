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

# This file defines global functions that create and initialize
# normalizer objects at the same time

import norma as cpp
import ChainNormalizer

class Mapper(cpp.MapperNormalizer):
    """Normalizer that uses a dictionary of word forms.

    This normalizer works by storing a dictionary, or "mapping", of
    word forms in the form "input_word" -> "normalized_word", together
    with counts how often this correspondence was seen in the training
    data.

    For normalization, it returns the "normalized_word" which was seen
    most often with the corresponding "input_word".  The score
    reflects the count of this mapping proportional to the count of
    all mappings for "input_word".

    """

    def __init__(self, *args):
        """Construct and initialize the normalizer.

        This constructor automatically calls init() with all supplied
        arguments.
        
        Optional arguments:
          file -- Name of the parameter file
        """
        super(Mapper, self).__init__()
        self.init(*args)

class Rulebased(cpp.RulebasedNormalizer):
    """Normalizer that uses context-aware character rewrite rules.

    Implements the normalization technique first described in:

    Marcel Bollmann, Florian Petran, and Stefanie Dipper
    (2011). *Rule-Based Normalization of Historical Texts.* In:
    Proceedings of the RANLP Workshop on Language Technologies for
    Digital Humanities and Cultural Heritage, pp. 34--42.  Hissar,
    Bulgaria.
    <http://www.linguistics.rub.de/~bollmann/pub/ranlp11.pdf>

    """
    
    def __init__(self, *args):
        """Construct and initialize the normalizer.

        This constructor automatically calls init() with all supplied
        arguments.
        
        Optional arguments:
          file -- Name of the parameter file
           lex -- Lexicon object to set for this normalizer
        """
        super(Rulebased, self).__init__()
        self.init(*args)

RuleBased = Rulebased

class WLD(cpp.WLDNormalizer):
    """Normalizer that uses a weighted Levenshtein distance measure.

    Implements the normalization technique described in section 3.1.4
    of:

    Marcel Bollmann (2013).  Automatic Normalization for Linguistic
    Annotation of Historical Language Data.  *Bochumer Linguistische
    Arbeitsberichte:* 13.
    <http://www.linguistics.rub.de/bla/013-bollmann2013.pdf>

    """
    
    def __init__(self, *args):
        """Construct and initialize the normalizer.

        This constructor automatically calls init() with all supplied
        arguments.
        
        Optional arguments:
          file -- Name of the parameter file
           lex -- Lexicon object to set for this normalizer
        """
        super(WLD, self).__init__()
        self.init(*args)

# Aliases for normalizers implemented in Python:
Chain = ChainNormalizer.ChainNormalizer
