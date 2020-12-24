/**
 * @file
 * @brief This file defines the Utils::Log::Private::Level typesafe enum
 * Also uses macros to determine which logs are enabled or not
 */
#ifndef __UTILS_LOG_PRIVATE_LEVEL_HPP__
#define __UTILS_LOG_PRIVATE_LEVEL_HPP__


/** A namespace used for the utils directory */
namespace Utils {

    /** A namespace used for logging functions
     *  Unless otherwise specified, each public logging function in this namespace takes in
     * whatever arguments it is given by copy, and returns void. There are no restrictions on what
     * types of arguments, or how many arguments are given, other than that the '<<' stream
     * operator must be defined for the type. Optionally, a class can be provided as an extra
     * template argument to log. If it is provided the log written to will be a custom log related
     * to that particular class. If no custom log is specified a default log is used.
     */
    namespace Log {

        /** A namespace used to designate certain items in Utils::Log as private
         *  These functions should not be called outside of Utils::Log members
         */
        namespace Private {

            /** A typesafe enum denoting different log levels */
            enum class Level { Debug, Verbose, Info, Warning, Error, Critical };

        } // namespace Private

    } // namespace Log

} // namespace Utils

#endif
