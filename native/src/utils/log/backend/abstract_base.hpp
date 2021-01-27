/**
 * @file
 * \ingroup utils
 * @brief This file defines the base logging backend class
 */
#ifndef __UTILS_LOG_BACKEND_ABSTRACTBASE_HPP__
#define __UTILS_LOG_BACKEND_ABSTRACTBASE_HPP__

#include "../../../constants.hpp"
#include "../../../macros.hpp"
#include "../level.hpp"

#include <string>


namespace Utils::Log::Backend {

    /** The base backend class
     *  All Log backend must subclass this
     *  The subclass must implement the log function defined below
     */
    struct AbstractBase {

        /** Default virtual destructor */
        virtual ~AbstractBase() = 0;

        // Rule of 5
        SET_IMPLICITS(AbstractBase, default)

        /** Log the given message, level, to the correct log given by log_id */
        virtual void log(Constants::CCSC id, const Level::Level &lvl, const std::string &msg) = 0;
    };

} // namespace Utils::Log::Backend

#endif
