/**
 * @file
 * @brief This file defines the AST::Cached::Bool class and defines AST::Bool
 */
#ifndef __AST_BOOL_HPP__
#define __AST_BOOL_HPP__

#include "using_declarations.hpp"

#include "../macros.hpp"

#include "base.hpp"


/** A namespace used for the ast directory */
namespace AST {

    /** A namespace to denote self-caching classes
     *  These classes are unlikely to be accessed directly, but rather should be accessed via a
     * shared_ptr
     */
    namespace Cached {

        /** This class represents an AST boolean */
        class Bool : public Base {
          public:
            /** Return true if the AST evaluates to true */
            bool is_true() const;

            /** Return true if the AST evaluates to false */
            bool is_false() const;

            /** Return the name of the type this class represents */
            std::string type_name() const;

          private:
            /** Delete all default constructors */
            DELETE_DEFAULTS(Bool)
        };
    } // namespace Cached

} // namespace AST

#endif
