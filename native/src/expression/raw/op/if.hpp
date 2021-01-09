/**
 * @file
 * @brief
 */
#ifndef __EXPRESSION_RAW_OP_IF_HPP__
#define __EXPRESSION_RAW_OP_IF_HPP__

#include "base.hpp"


namespace Expression::Raw::Op {

    /** The op class If */
    class If : virtual public Base {
        EXPRESSION_RAW_ABSTRACT_INIT(If)
      public:
        /** Pure virtual destructor */
        virtual ~If() = 0;

        /** Return the op */
        Constants::CCS op() const override final;

      protected:
        /** A protected constructor to disallow public creation */
        If() = default;
    };

} // namespace Expression::Raw::Op

#endif
