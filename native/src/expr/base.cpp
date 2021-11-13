/** @file */
#include "base.hpp"

#include "../op.hpp"


void Expr::Base::ctor_debug_checks() const {
    using E = Util::Err::Usage;
    if (op->cuid == Op::Symbol::static_cuid) {
        Util::affirm<E>(symbolic, WHOAMI_HEADER_ONLY "Symbolic Op may not be concrete");
    }
    else if (op->cuid == Op::Literal::static_cuid) {
        Util::affirm<E>(!symbolic, WHOAMI_HEADER_ONLY "Literal Op may not be symbolic");
    }
}

void Expr::Base::repr(std::ostream &out) const {
    UTIL_AFFIRM_NOT_NULL_DEBUG(op); // Sanity check
    out << R"|({ "type":")|" << type_name();
    out << R"|(", "symbolic":)|" << std::boolalpha << symbolic << ", ";
    if (const auto *const bits { dynamic_cast<CTSC<Expr::Bits>>(this) }; bits != nullptr) {
        out << R"|("bit_length":)|" << bits->bit_length << ", ";
    }
    out << R"|("op":)|";
    op->repr(out);
    if (annotations != nullptr) {
        out << R"|(, "annotations":)|";
        annotations->repr(out);
    }
    out << " }";
}

std::string Expr::Base::repr() const {
    std::ostringstream o;
    repr(o);
    return o.str();
}