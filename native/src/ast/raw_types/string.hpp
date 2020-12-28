/**
 * @file
 * @brief This file defines the AST::RawTypes::String class and defines AST::String
 */
#ifndef __AST_RAWTYPES_STRING_HPP__
#define __AST_RAWTYPES_STRING_HPP__

#include "bits.hpp"


namespace AST {

    // These types should be wrapped by a shared pointer when used
    // A factory is used to construct them and handle hash caching
    namespace RawTypes {

        /** An AST representing a string */
        class String : public Bits {
            AST_RAWTYPES_INIT_AST_BASE_SUBCLASS(String)

            /** Create a concrete String
             *  @todo kwargs
             */
            /* static ::AST::String Concrete(const std::string & value, const Constants::Int
             * length); */

            /** A protected constructor to disallow public creation
             *  This must have take in the same arguments types as the hash function, minus the
             * hash These arguments may be taken in via copy, reference or move; ownership is given
             */
            String(const Hash h, const Ops::Operation o);

            /** The hash function of this AST
             *  This must have take in the same arguments as the constructor, minus the hash
             *  These arguments args must be const values or references; this function must be pure
             */
            static Hash hash(const Ops::Operation o);
        };

    } // namespace RawTypes

} // namespace AST

#endif