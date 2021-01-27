/**
 * @file
 * @brief This file defines the Expression::Raw::Type::FP class
 */
#ifndef __EXPRESSION_RAW_TYPE_FP_HPP__
#define __EXPRESSION_RAW_TYPE_FP_HPP__

#include "bits.hpp"


namespace Expression::Raw::Type {

    /** An Expression representing an integer */
    class FP : virtual public Bits {
        EXPRESSION_RAW_ABSTRACT_INIT_IMPLICIT_CTOR(FP)
      public:
        /** Get the type of the expression */
        Constants::CCS type() const override final;
    };

} // namespace Expression::Raw::Type

#endif
