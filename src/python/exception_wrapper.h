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
#ifndef NORMA_PYTHON_EXCEPTION_WRAPPER_H_
#define NORMA_PYTHON_EXCEPTION_WRAPPER_H_
#include<boost/python.hpp>  //NOLINT[build/include_order]
#include"normalizer/exceptions.h"

namespace Norma {
namespace Python {
void translate_norma_exception(const Norma::Normalizer::init_error& e);

void register_exception_translators();
}  // namespace Python
}  // namespace Norma

#endif  // NORMA_PYTHON_EXCEPTION_WRAPPER_H_
