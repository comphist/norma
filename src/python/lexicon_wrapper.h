/* Copyright 2013-2015 Marcel Bollmann, Florian Petran
 *
 * This file is part of Norma.
 *
 * Norma is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * Norma is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with Norma.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef NORMA_PYTHON_LEXICON_WRAPPER_H_
#define NORMA_PYTHON_LEXICON_WRAPPER_H_
#include<string>

namespace Norma {
namespace Normalizer {
class Lexicon;
}  // namespace Normalizer

namespace Python {
struct lexicon_wrapper {
    static void init(Norma::Normalizer::Lexicon* l,
                     std::string lexfile, std::string symfile);
    static void save(Norma::Normalizer::Lexicon* l,
                     std::string lexfile, std::string symfile);
    static void wrap();
};
}  // namespace Python
}  // namespace Norma

#endif  // NORMA_PYTHON_LEXICON_WRAPPER_H_
