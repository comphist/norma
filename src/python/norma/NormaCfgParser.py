#!/usr/bin/python
# -*- coding: utf-8 -*-
################################################################################
# Copyright 2013-2016 Marcel Bollmann, Florian Petran
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

from __future__ import print_function, division, absolute_import, unicode_literals
import os, sys
import ConfigParser
from norma.LexiconWrapper import Lexicon
import norma.NormalizerWrapper as Normalizer

class InitError(Exception):
    pass

class FakeSecHead(object):
    def __init__(self, fp):
        self.fp = fp
        self.sechead = '[General]\n'

    def readline(self):
        if self.sechead:
            try:
                return self.sechead
            finally:
                self.sechead = None
        else:
            return self.fp.readline()

class NormaCfgParser(object):
    cfgpath = "."
    cfg = {
        'interactive': True,
        'verbose': True,
        'saveonexit': False,
        'perfilemode': False,
        'logging': True
        }
    lexinfo  = {
        'fsmfile': None,
        'symfile': None
    }
    norminfo = []

    def __init__(self, config=None, errout=sys.stderr):
        self.errout = errout
        if (config is not None):
            self.norminfo = self.load_config(config)

    def interpret_path(self, path):
        return str(self.cfgpath + "/" + path)

    def _write_error(self, err):
        if self.errout is not None:
            self.errout.write(err)

    def instantiate_all(self, with_chain=False):
        lexicon = Lexicon(self.interpret_path(self.lexinfo['fsmfile']),
                          self.interpret_path(self.lexinfo['symfile']))
        normalizers = []
        for data in self.norminfo:
            name = data[0].encode("utf8")
            if name == 'Mapper' and 'mapfile' in data[1]:
                mapfile = self.interpret_path(data[1]['mapfile'])
                normalizer = Normalizer.Mapper(mapfile, lexicon)
            elif name == 'RuleBased' and 'rulesfile' in data[1]:
                rulesfile = self.interpret_path(data[1]['rulesfile'])
                normalizer = Normalizer.Rulebased(rulesfile, lexicon)
            elif name == 'WLD' and 'paramfile' in data[1]:
                paramfile = self.interpret_path(data[1]['paramfile'])
                wld = Normalizer.WLD()
                wld.lexicon = lexicon
                wld.paramfile = paramfile
                if 'max_weight' in data[1]:
                    wld.max_weight = float(data[1]['max_weight'])
                if 'max_ops' in data[1]:
                    wld.max_ops = int(data[1]['max_ops'])
                if 'train_ngrams' in data[1]:
                    wld.ngrams = int(data[1]['train_ngrams'])
                if 'train_divisor' in data[1]:
                    wld.divisor = int(data[1]['train_divisor'])
                wld.init()
                normalizer = wld
            else:
                raise InitError("Unknown normalizer: %s" % name)
            normalizer.name = name
            normalizers.append(normalizer)
        if with_chain and len(normalizers) > 0:
            chain = Normalizer.Chain(*normalizers)
            normalizers.append(chain)
        return normalizers

    def load_config(self, config):
        """Load and parse a configuration file, setting program
        variables accordingly.  Returns a list of tuples containing
        information about the normalizers to instantiate, the first
        element being its name, the second a list of its
        parameters."""
        # Check if file exists
        if (config is None) or (config==""):
            self._write_error("*** WARNING: No configuration file specified.\n")
            return False
        if not os.path.exists(config):
            raise InitError("Configuration file not found: '%s'"%config)

        self.cfgpath = os.path.dirname(os.path.abspath(config))

        try:
            normalizer_info = self._parse_config(config)
        except ConfigParser.Error as detail:
            raise InitError("While processing configuration file '%s':%s" % (config, detail))
            return False
        else:
            if self.cfg['verbose']:
                self._write_error("*** Loaded configuration file '%s'.\n" % config)
            return normalizer_info

    def _parse_config(self, config):
        """Parse a configuration file and set variables accordingly.
        Called from load_config(), where all parameter checking and
        error handling should be done."""
        cfg = ConfigParser.SafeConfigParser()
        with open(config, 'r') as cfile:
            cfg.readfp(FakeSecHead(cfile))

        # Set boolean options
        boolean_options = ['interactive','verbose','saveonexit','perfilemode','logging']
        for option in boolean_options:
            if cfg.has_option("General", option):
                self.cfg[option] = cfg.getboolean("General", option)

        if cfg.has_section("Lexicon"):
            self.lexinfo = dict(cfg.items("Lexicon"))
        if ('fsmfile' not in self.lexinfo) or ('symfile' not in self.lexinfo):
            self._write_error("*** WARNING: Lexicon not specified.\n")

        # Collect information about the normalizers
        if cfg.has_option("General", "normalizers"):
            normalizer_info = []
            x = cfg.get("General", "normalizers").strip()
            if x:
                for normalizer in x.split(","):
                    if cfg.has_section(normalizer):
                        params = dict(cfg.items(normalizer))
                    else:
                        params = {}
                    normalizer_info.append( (normalizer, params) )

        return normalizer_info
