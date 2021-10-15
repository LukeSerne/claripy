/**
 * @file
 * @brief This file defines BigInt modes
 */
#ifndef R_MODE_BIGINT_HPP_
#define R_MODE_BIGINT_HPP_

namespace Mode {

    /** A mask used to define the type of comparison to be used */
    enum class BigInt { Int, Str };

    /** Ostream operator for Mode::BigInt */
    inline std::ostream &operator<<(std::ostream &os, const BigInt &b) {
        return os << "Mode::BigInt::" << ((b == BigInt::Int) ? "Int" : "Str");
    }

} // namespace Mode

#endif