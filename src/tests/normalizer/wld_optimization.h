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
#include"normalizer/wld.h"
#include"normalizer/result.h"
#include"string_impl.h"

namespace {  //NOLINT[build/namespaces]
  // Derived class to allow manipulation of the internal cascade
  class BenchmarkWLD : public Norma::Normalizer::WLD::WLD {
   public:
    void set_max_weight(double w) { _cascade->set_max_weight(w); }
    void set_max_ops(unsigned int n) { _cascade->set_max_ops(n); }
    Norma::Normalizer::Result operator()(const string_impl& word) const;
  };
}
