/**
 * @file
 * @brief This file defines a symbolic expression
 */
#ifndef __EXPRESSION_RAW_SYMBOLIC_HPP__
#define __EXPRESSION_RAW_SYMBOLIC_HPP__

#include "base.hpp"


namespace Expression {

    namespace Raw {

        /** A symbolic expression
         *  All symbolic expressions must subclass this
         */
        struct Symbolic : virtual public Base {
            EXPRESSION_RAW_ABSTRACT_INIT(Symbolic)
          public:
            /** Pure virtual destructor */
            virtual ~Symbolic() = 0;

            /** Return true if and only if this expression is symbolic */
            bool symbolic() const override final;

          protected:
            /** Disallow public construction */
            Symbolic() = default;
        };

    } // namespace Raw

    // Create a shared pointer alias
    EXPRESSION_RAW_DECLARE_SHARED(Symbolic, Raw)

} // namespace Expression

#endif
