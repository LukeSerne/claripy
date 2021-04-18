/**
 * @file
 * @brief This file defines the Op class FP::ToBV
 */
#ifndef __OP_FP_TOBV_HPP__
#define __OP_FP_TOBV_HPP__

#include "../../mode.hpp"
#include "../base.hpp"


namespace Op::FP {

    /** The op class: to_bv */
    template <bool Signed> class ToBV final : public Base {
        OP_FINAL_INIT(ToBV, "FP::");

      public:
        /** The FP mode */
        const Mode::FP mode;
        /** The fp to convert: This must be an Expression::BV pointer
         *  Note: We leave it as a base for optimizations purposes
         */
        const Expression::BasePtr fp;

        /** Add's the raw expression children of the expression to the given stack in reverse
         *  Warning: This does *not* give ownership, it transfers raw pointers
         */
        inline void add_reversed_children(Stack &s) const override final { s.emplace(fp.get()); }

      private:
        /** Protected constructor
         *  Ensure that fp is an FP
         */
        explicit inline ToBV(const Hash::Hash &h, const Mode::FP m, const Expression::BasePtr &f)
            : Base { h, static_cuid }, mode { m }, fp { f } {
            Utils::affirm<Error::Expression::Type>(CUID::is_t<Expression::FP>(fp),
                                                   WHOAMI_WITH_SOURCE
                                                   "Operand fp must be an Expression::FP");
        }
    };

} // namespace Op::FP

#endif
