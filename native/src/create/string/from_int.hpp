/**
 * @file
 * @brief This file defines a creation method for an expr containing String::FromInt
 */
#ifndef R_CREATE_STRING_FROMINT_HPP_
#define R_CREATE_STRING_FROMINT_HPP_

#include "../constants.hpp"


namespace Create::String {

    /** Create an Expr with a String::FromInt op
     *  Note: Currently Ints are taken in as BVs
     *  Note: For now, we just set the size to 2 bytes larger than the input
     *  This should be large-enough, and isn't as bad an over-estimation as INT_MAX or anything
     *  Expr pointers may not be nullptr
     */
    inline Expr::BasePtr from_int(const Expr::BasePtr &x, Annotation::SPAV &&sp = nullptr) {
        namespace Ex = Expr;
        namespace Err = Error::Expr;
        Util::affirm<Err::Usage>(x != nullptr,
                                 WHOAMI_WITH_SOURCE "Expr pointers cannot be nullptr");
        Util::affirm<Err::Type>(CUID::is_t<Ex::BV>(x),
                                WHOAMI_WITH_SOURCE "operand must be each be of type Expr::BV");
        return Simplification::simplify(Ex::factory<Ex::String>(
            x->symbolic, Op::factory<Op::String::FromInt>(x),
            Ex::get_bit_length(x) + 2_ui * BitLength::char_bit, std::move(sp)));
    }

} // namespace Create::String

#endif
