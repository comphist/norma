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

class ChainNormalizer(list):
    """Normalizer that represents a chain of other normalizers.

    This normalizer acts as an ordered list of (other) normalizers.
    For normalization, normalizers are called in the given order, and
    the first result with a score above a certain threshold is
    returned.  Normalizers further down the list are never called.

    For training, the training data is passed to all contained
    normalizers.

    Member variables:
      threshold -- Minimum score for returned normalization candidates
        lexicon -- A common Lexicon object for all normalizers in the
                   chain (set this after adding all normalizers)

    """
    
    _name = "Chain"
    _lexicon = None

    threshold = 0

    def __init__(self, *normalizers):
        """Construct a chain of normalizers.

        Optional arguments:
          Any number of normalizers that should be appended to the chain.
        """
        self.init(*normalizers)
            
    def init(self, *normalizers):
        """Initialize a chain of normalizers.

        Optional arguments:
          Any number of normalizers that should be appended to the chain.
        """
        self[:] = []
        for norm in normalizers:
            self.append(norm)

    def clear(self):
        """Clear all normalizers from the chain."""
        self[:] = []

    def save(self):
        """Call the save() method on all normalizers in the chain."""
        for norm in self:
            norm.save()

    def _normalize_best(self, word):
        for norm in self:
            r = norm(word)
            if r[1] > self.threshold:
                return r
        return (word, 0.0, '[None]')
            
    def _normalize_n_best(self, word, n):
        from operator import itemgetter
        results = []
        for norm in self:
            results.extend(norm(word, n))
        results = [r for r in results if r[1] > self.threshold]
        return sorted(results, key=itemgetter(1), reverse=True)[:n]

    def normalize(self, word, n=None):
        """Normalize a word and return the best (or n best) candidate(s).

        If n was omitted, calls the normalizers sequentially until the
        first normalizer returns a result with score > self.threshold;
        otherwise, calls all normalizers and returns the n best
        candidates in descending order of their scores.
        
        Arguments:
          word -- The string that should be normalized
             n -- (optional) How many candidates to return

        Returns:
          If n was omitted, a tuple of the form (<normalized_word>,
          <score>, self.name); otherwise a list of up to n such tuples.

        """
        if n is None:
            return self._normalize_best(word)
        else:
            return self._normalize_n_best(word, n)
    
    def __call__(self, word, n=None):
        """Alias for normalize(word, n=None)"""
        return self.normalize(word, n)

    def train(self, data):
        """Train all normalizers in the chain on a list of word forms.

        Arguments:
          data -- A list of tuples (<input_word>, <normalized_word>)
        """
        for norm in self:
            norm.train(data)

    @property
    def name(self):
        return self._name
    
    @property
    def lexicon(self):
        return self._lexicon

    @lexicon.setter
    def lexicon(self, value):
        self._lexicon = value
        for norm in self:
            norm.lexicon = value
