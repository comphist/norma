# Norma

[![Build Status](https://travis-ci.org/comphist/norma.svg?branch=master)](https://travis-ci.org/comphist/norma)
[![codecov](https://codecov.io/gh/comphist/norma/branch/master/graph/badge.svg)](https://codecov.io/gh/comphist/norma)

Norma is a tool for **automatic spelling normalization** of non-standard language data.  It
uses a combination of different normalization techniques that typically require
*training data* (= a list of manually normalized wordforms) and a *target
dictionary* (= a list of valid wordforms in the target language).

Besides this README, see also the [User Guide](doc/UserGuide.md) for more
information on how to use Norma.

If you have any questions, suggestions, or comments, please contact one of the authors:

* Marcel Bollmann (<marcel@bollmann.me>)
* Florian Petran (<florian.petran@gmail.com>)

#### License

Norma is licensed under the
[GNU Lesser General Public License (LGPL) v3](http://www.gnu.org/licenses/lgpl-3.0).

#### Usage via Docker

If you don't want to compile Norma and its dependencies from scratch, you can
try to use it via a Docker image.  To do so, install
[Docker](https://www.docker.com/) (e.g. via your system's package manager) and
run:

    docker run -v $(pwd):/home mbollmann/norma

This should download the Docker image and display Norma's help output.  You can
now add command-line arguments the same way as with the `normalize` binary; for
example, if you cloned this repository locally, you should be able to run:

    docker run -v $(pwd):/home mbollmann/norma -s -c doc/example/example.cfg -f doc/example/fnhd_sample.txt

Make sure that the files you specify exist within your current working directory.

#### Dependencies

* Needed for compilation:
    * GCC >= 4.9
    * CMake >= 2.8.10
    * Boost >= 1.54
        * in particular these libraries: Filesystem, Program Options, Regex, System, Test
    * pkg-config
    * gfsm >= 0.0.16-1 and gfsmxl >= 0.0.15,
      available from http://kaskade.dwds.de/~moocow/mirror/projects/gfsm/
    * GLib >= 2.0
* Optionally:
    * ICU >= 1.49
    * Doxygen (for generating the documentation)
    * Python 2 >= 2.7 and Boost::Python (for Python bindings/embeddings)

#### How to do an out of source build

    mkdir build
    cd build
    cmake <pathtosource>
    make

There is a test suite included that can be run with `make test` (or `make check`, which reveals more detailed error
messages), as well as API documentation that is generated via `make doc` (requires Doxygen).

#### Configuration options (for CMake)

* String implementation (default: ICU if available):
    `-DSTRING_IMPL=(ICU|STD)`
    * ICU - use ICU unicode strings
    * STD - use STL string - requires no additional library
* Build type (default: Release):
    `-DCMAKE_BUILD_TYPE=(Debug|Release):`
* Install prefix (default: /usr/local/)
    `-DCMAKE_INSTALL_PREFIX=<prefix>`
* To make Python bindings/embeddings (default: disabled), set
    `-DWITH_PYTHON=TRUE`

#### Other platforms

* Norma was developed on Linux and will not work on non-Unix platforms.
* Compilation with clang will probably work but is untested.
