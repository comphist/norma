#!/usr/bin/python
# -*- coding: utf-8 -*-

from __future__ import print_function, division, absolute_import, unicode_literals
import argparse
import sys
from norma import NormaCfgParser
from pprint import pprint

class MainApplication(object):
    def __init__(self, args):
        self.normalizers = NormaCfgParser(args.config).instantiate_all()
        self.normalizer_names = map((lambda n: n.name), self.normalizers)
        self.say("Instantiated %i normalizers:\n" % len(self.normalizers))
        self.say("  %s\n" % ', '.join(self.normalizer_names))
        self.infile = args.infile
        self.encoding = args.encoding
        self.candidates = args.candidates

    def say(self, text, to=sys.stdout):
        try:
            to.write(text.encode("utf-8"))
        except UnicodeError:
            to.write(text)

    def run(self):
        def make_result(r):
            return (r.word, r.score, r.origin)

        results = []
        for line in self.infile:
            orig = line.decode(self.encoding).strip()

            current = {'orig': orig}
            for normalizer in self.normalizers:
                resultset = normalizer(orig, self.candidates)
                current[normalizer.name] = [make_result(r) for r in resultset]
            results.append(current)
            pprint(current)

        self.say("Processed %i tokens.\n" % len(results), to=sys.stderr)
        self.say("Done!\n", to=sys.stderr)

if __name__ == '__main__':
    description = "Generates normalizations for a test set."
    epilog = ""
    parser = argparse.ArgumentParser(description=description, epilog=epilog)
    parser.add_argument('infile',
                        metavar='INPUT',
                        type=argparse.FileType('r'),
                        help='Test set')
    parser.add_argument('-c', '--config',
                        metavar='CONFIG',
                        required=True,
                        help='Norma configuration file')
    parser.add_argument('-n', '--candidates',
                        metavar='N',
                        type=int,
                        default=3,
                        help='Number of normalization candidates to generate (default: %(default)i)')
    parser.add_argument('-e', '--encoding',
                        default='utf-8',
                        help='Encoding of the input file (default: utf-8)')

    args = parser.parse_args()

    # launching application ...
    MainApplication(args).run()
