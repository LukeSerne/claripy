/**
 * @file
 * @brief This file defines FP modes
 */
#ifndef R_MODE_FP_ROUNDING_HPP_
#define R_MODE_FP_ROUNDING_HPP_

namespace Mode::FP {

    /** FP modes supported by claripy */
    enum class Rounding {
        NearestTiesEven = 0,
        NearestTiesAwayFromZero,
        TowardsZero,
        TowardsPositiveInf,
        TowardsNegativeInf
    };

} // namespace Mode::FP

#endif
