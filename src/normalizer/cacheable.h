/* Copyright 2013-2016 Marcel Bollmann, Florian Petran
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
#ifndef NORMALIZER_CACHEABLE_H_
#define NORMALIZER_CACHEABLE_H_
#include<atomic>
#include<map>
#include<shared_mutex>
#include"normalizer/result.h"
#include"string_impl.h"

namespace Norma {
namespace Normalizer {
class Cacheable {
 public:
     void set_caching(bool value);
     bool is_caching() const { return _caching.load(); }
     void clear_cache() const;

 protected:
     Result query_cache(const string_impl& word) const;
     void cache(const string_impl& word, const Result& result) const;

 private:
     std::atomic_bool _caching {true};
     mutable std::map<string_impl, Result> _cache;
     mutable std::shared_timed_mutex cache_mutex;
};
}  // namespace Normalizer
}  // namespace Norma

#endif  // NORMALIZER_CACHEABLE_H_

