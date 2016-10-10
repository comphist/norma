# Python Bindings for Norma
## Introduction

Norma provides bindings for Python 2.x to facilitate the use of its
normalization algorithms within larger projects.  Building the
bindings requires the Boost::Python library and the cmake flag
`-DWITH_PYTHON`.

### Unicode Support

Compilation with ICU strings (`-DSTRING_IMPL=ICU`) is **strongly
recommended** for proper Unicode support.

When Norma is compiled with ICU,
- you can safely use Python `unicode` strings with Norma;
- byte strings (`str`) are assumed to be encoded in UTF-8; and
- Norma methods will return `unicode` strings.

When Norma is compiled with STL strings,
- `unicode` strings can only contain characters that can be encoded in Latin1, and will raise a `UnicodeEncodeError` otherwise;
- byte strings (`str`) are copied verbatim, and any multi-byte characters will not be recognized as such and therefore likely not behave as expected; and
- Norma methods will return byte strings, whose encoding depends on what the encoding of its input was.

## Usage Example

These bindings provide a Python package called `norma` with the
following modules and classes:

- LexiconWrapper
  + Lexicon - exposes a lexicon object
- NormalizerWrapper
  + Mapper - exposes the mapper normalizer
  + Rulebased - exposes the rule-based normalizer
  + WLD - exposes the WLD normalizer
  + Chain - container holding a chain of normalizers, implemented in Python

Most classes and functions are also documented via Python's docstring
functionality.

### Using the Lexicon

A Lexicon object can be constructed and used like this:

    >>> from norma.LexiconWrapper import Lexicon
    >>> lex = Lexicon()
    >>> lex.add("foo", "bar", "baz")
    >>> "bar" in lex
    True
    >>> len(lex)
    3

Saving and loading the lexicon to/from the specified files:

    >>> lex.save("my_lexicon.fsm", "my_lexicon.sym")
    >>> lex = Lexicon("my_lexicon.fsm", "my_lexicon.sym")

The main usefulness of the Lexicon, though, is in supplying it as a
parameter to normalizers.

### Using the normalizers

Assuming `lex` contains a previously instantiated Lexicon object, here
is an example how to use the rule-based normalizer:

    >>> import norma.NormalizerWrapper as Normalizer
    >>> norm = Normalizer.Rulebased()
    >>> norm.name = "Rulebased"  # unfortunately currently needed
    >>> norm.lexicon = lex
    >>> norm.train([('drey', 'drei'), ('frevde', 'freude')])
    True
    >>> norm('drey')
    Result('drei', 0.5294117647058824, 'RuleBased')
    >>> norm('frevde')
    Result('freude', 0.52, 'RuleBased')
    >>> norm('frey')
    Result('frei', 0.5294117647058824, 'RuleBased')

The individual components of a `Result` object can be accessed via properties:

    >>> r = norm('frey')
    >>> r.word
    'frei'
    >>> r.score
    0.5294117647058824
    >>> r.origin
    'RuleBased'

Saving and loading works in a similar fashion as above:

    >>> norm.save('myrules.txt')
    >>> norm = Normalizer.Rulebased('myrules.txt', lex)

**Attention:** The WLD normalizer currently has the special quirk that
  you must call `norm.perform_training()` to actually trigger the
  training algorithm.  This behaviour might change in the future.

The Chain normalizer is not ported from C++, but implemented in
Python.  It can be used to simulate parts of the functionality of the
Applicator class in C++.  Assuming you have instantiated different
normalizers `norm1`, `norm2` and `norm3`:

    >>> chain = Normalizer.Chain(norm1, norm2, norm3)
    >>> chain('jn')
    ('in', 0.75, 'Mapper')

Here, the last element of the returned tuple is actually useful as it
provides info about which normalizer generated the best result.  You
can also supply a second argument when invoking any normalizer to get
more than one result:

    >>> chain('jn', 2)
    [('in', 0.75, 'Mapper'), ('ihn', 0.5, 'RuleBased')]

### Instantiating normalizers from config file

You can also instantiate normalizers from the same configuration file that the
command-line tool uses, like this:

    >>> from norma import NormaCfgParser
    >>> normalizers = NormaCfgParser('example_chain.cfg').instantiate_all()
    >>> for norm in normalizers:
    ...     norm('drey')
    ...
    Result('drei', 1, 'Mapper')
    Result('drei', 0.332345, 'RuleBased')
    Result('drei', 0.75952, 'WLD')

A more detailed example script is provided in `example/normalize.py`.

### Handling exceptions

Both lexicon and normalizers can throw a `NormaInitError` during initialisation
when an unexpected, but non-fatal situation occurs.  Most commonly, this can
happen if they are initialised with non-existant filenames for their parameter
files.  This can be intentional, e.g. if the parameter files do not exist yet
but should be created after training, or it could be an actual error, e.g. if
you misspelled a filename.

In any case, a `NormaInitError` always leaves the lexicon or normalizer in a
valid state.  Fatal errors that prevent normal execution will throw a generic
`RuntimeError` instead.
