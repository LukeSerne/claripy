/**
 * @file
 * \ingroup unittest
 */
#include "testlib.hpp"

#include <set>


// For brevity
using namespace Expression;
using namespace UnitTest::TestLib;


namespace UnitTest {
    /** A struct used to give friend access to unittests */
    struct ClaricppUnitTest {
        /** Get the cache size */
        decltype(Expression::Private::cache)::CacheMap::size_type size() {
            return Expression::Private::cache.cache.size();
        }
        /** Passthrough unsafe_find */
        Base unsafe_find(const Hash::Hash &h) { return Expression::Private::cache.unsafe_find(h); }
    };
} // namespace UnitTest


/** Ensure weak_ptrs are properly invalidated and removed by both gc and find */
int weak_ptr_invalidation_find() {
    UnitTest::ClaricppUnitTest cache;

    // Create and destroy a base, but record its hash
    Hash::Hash id;
    {
        auto a = literal_int();
        id = a->id;
    }

    // Check cache and cache size
    UNITTEST_ASSERT(cache.size() == 1)
    UNITTEST_ASSERT(cache.unsafe_find(id) == nullptr);
    return cache.size();
}
