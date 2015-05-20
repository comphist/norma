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

