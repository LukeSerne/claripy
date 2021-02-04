/**
 * @file
 * @brief This file defines macros used across the op directory
 */
#ifndef __OP_MACROS_HPP__
#define __OP_MACROS_HPP__

#include "../constants.hpp"
#include "../utils.hpp"


/** Initalize a final op class
 *  Leaves the class in a private access context
 */
#define OP_FINAL_INIT(CLASS)                                                                      \
  public:                                                                                         \
    /* Delete implicits */                                                                        \
    SET_IMPLICITS_CONST_MEMBERS(CLASS, delete)                                                    \
    /** Default destructor */                                                                     \
    ~CLASS() noexcept override final = default;                                                   \
    FACTORY_ENABLE_CONSTRUCTION_FROM_BASE(::Op::Base)                                             \
  private:


#endif
