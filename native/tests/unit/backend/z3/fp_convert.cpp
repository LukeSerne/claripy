/**
 * @file
 * \ingroup unittest
 */
#include "backend.hpp"
#include "create.hpp"
#include "testlib.hpp"

#include <cmath>


/** Return true iff p evaluates to true */
inline bool is_true(Backend::Z3::Z3 &z3, const Expression::BasePtr &p) {
    return z3.convert(p.get()).simplify().is_true();
}

/** Return true iff p evaluates to false */
inline bool is_false(Backend::Z3::Z3 &z3, const Expression::BasePtr &p) {
    return z3.convert(p.get()).simplify().is_false();
}


/** Try to convert a claricpp expression to z3 */
void fp_convert() {
    namespace Ex = Expression;
    using AV = Ex::Base::AnVec;
    namespace C = Create;

    auto z3 { Backend::Z3::Z3 {} };
    auto solver { z3.new_tls_solver() };

    // Test with Nan
    const auto nan { C::literal(AV {}, std::numeric_limits<double>::quiet_NaN()) };
    const auto nan_is_nan { C::FP::is_nan(AV {}, nan) };
    const auto nan_is_inf { C::FP::is_inf(AV {}, nan) };

    UNITTEST_ASSERT(is_true(z3, nan_is_nan))
    UNITTEST_ASSERT(is_false(z3, nan_is_inf))

    // Test with Inf
    const auto inf { C::literal(AV {}, std::numeric_limits<double>::infinity()) };
    const auto inf_is_nan { C::FP::is_nan(AV {}, inf) };
    const auto inf_is_inf { C::FP::is_inf(AV {}, inf) };
    UNITTEST_ASSERT(is_false(z3, inf_is_nan))
    UNITTEST_ASSERT(is_true(z3, inf_is_inf))

    // Test float
    const auto flt { C::literal(AV {}, 0.f) }; // NOLINT
    const auto flt_conv { z3.convert(flt.get()) };
    const auto srt { flt_conv.get_sort() };
    const Backend::Z3::FP::Width fpa_srt {
        srt.fpa_ebits(),
        srt.fpa_sbits(),
    };
    UNITTEST_ASSERT(fpa_srt == Backend::Z3::FP::flt);
}

// Define the test
UNITTEST_DEFINE_MAIN_TEST(fp_convert)