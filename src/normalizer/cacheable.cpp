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
#include"cacheable.h"
#include<shared_mutex>
#include<mutex>
#include<atomic>
#include"normalizer/result.h"
#include"string_impl.h"

namespace Norma {
namespace Normalizer {
void Cacheable::set_caching(bool value) {
    _caching.store(value);
    if (!_caching)
        clear_cache();
}

void Cacheable::clear_cache() const {
    std::unique_lock<std::shared_timed_mutex> write_lock(cache_mutex);
    _cache.clear();
}

Result Cacheable::query_cache(const string_impl& word) const {
    std::shared_lock<std::shared_timed_mutex> read_lock(cache_mutex);
    // _cache has to be checked here, because if we checked from the
    // outside, it's possible another thread has changed our state between
    // the call of has_cached() and query_cache()
    if (_cache.count(word) > 0)
        return _cache.at(word);
    return Result();
}

void Cacheable::cache(const string_impl& word, const Result& result) const {
    std::unique_lock<std::shared_timed_mutex> write_lock(cache_mutex);
    _cache[word] = result;
}
}  // namespace Normalizer
}  // namespace Norma

