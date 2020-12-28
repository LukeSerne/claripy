/** @file */

#include "ast/bits.hpp"

#include "ast/factory.hpp"
#include "ops/operations.hpp"

#include <set>

/** Construct a Bits */
AST::Bits construct(Ops::Operation o) {
    std::set<AST::BackendID> s;
    return AST::factory<AST::Bits>(std::move(s), std::move(o), std::move(0));
}

/** Test creating an AST::Bits */
int bits_bits() {
    AST::Bits a = construct((Ops::Operation) 0);
    AST::Bits b = construct((Ops::Operation) 1);
    if (a != b) {
        return 0;
    }
    else {
        return 1;
    }
}