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

