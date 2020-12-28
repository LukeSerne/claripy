/**
 * @file
 * @brief This file contains the possible standard python exceptions claricpp can raise
 * These exceptions have python analogs and must be caught and set to python
 * via a different method.
 * @todo Document method when known
 */
#ifndef __ERRORS_PYTHON_HPP__
#define __ERRORS_PYTHON_HPP__

#include "claricpp.hpp"


namespace Utils::Error {

    // Exceptions that should be passed back to python
    namespace Python {

        /** Base Python exception
         *  All Python exceptions must derive from this
         */
        DEFINE_SUBCLASS_WITH_CONSTRUCTOR(Base, Claricpp)

        /** Analogous to python's ValueError exception */
        DEFINE_SUBCLASS_WITH_CONSTRUCTOR(ValueError, Base)
    } // namespace Python

} // namespace Utils::Error

#endif