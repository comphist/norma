# Norma

Norma is a tool for **automatic spelling normalization** of non-standard language data.  It
uses a combination of different normalization techniques that typically require
*training data* (= a list of manually normalized wordforms) and a *target
dictionary* (= a list of valid wordforms in the target language).

If you have any questions, suggestions, or comments, please contact one of the authors:

* Marcel Bollmann (<bollmann@linguistics.rub.de>)
* Florian Petran (<petran@linguistics.rub.de>)

#### License

Norma is licensed under the
[GNU Lesser General Public License (LGPL) v3](http://www.gnu.org/licenses/lgpl-3.0).

#### Dependencies

* Needed for compilation:
    * GCC >= 4.8
    * CMake >= 2.8.10
    * Boost >= 1.50
        * in particular these libraries: Filesystem, Program Options, Regex, System, Test
    * gfsm >= 0.0.11 and gfsmxl >= 0.0.11,
      available from http://kaskade.dwds.de/~moocow/mirror/projects/gfsm/
    * GLib >= 2.0
* Optionally:
    * ICU >= 1.49 (finding ICU will only work on OS that have pkgconfig)
    * Doxygen (for generating the documentation)
    * Python 2 >= 2.7 and Boost::Python (for Python bindings/embeddings)

#### How to do an out of source build

    mkdir build
    cd build
    cmake <pathtosource>
    make

There is a test suite included that can be run with `make test` (or `make check`, which reveals more detailed error messages), as well as some
rudimentary documentation that is generated via `make doc` (requires Doxygen).

#### Configuration options (for CMake)

* String implementation (default: ICU if available):
    `-DSTRING_IMPL=(ICU|STD)`
    * ICU - use ICU unicode strings
    * STD - use STL string - requires no additional library
* Build type (default: Release):
    `-DCMAKE_BUILD_TYPE=(Debug|Release):`
* Install prefix (default: /usr/local/)
    `-DDESTINATION=<prefix>`
* To make Python bindings/embeddings (default: disabled), set
    `-DUSE_PYTHON`

#### Other platforms

* Norma was developed on Linux but should theoretically work on other platforms
* Compilation with clang will probably work but is untested.
* Compilation on OSX will probably work but is untested
* Compilation on Windows might work if Microsoft ever releases a compiler that fully supports C++11 or if you use gcc.
* The main problem on either of those platforms will probably be to get the dependencies to build and get cmake to find them in the proper location.