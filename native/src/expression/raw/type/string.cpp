/** @file */
#include "string.hpp"

#include "../../../utils.hpp"


// For clarity
using namespace Expression::Raw;
using namespace Type;


String::~String() {}

Constants::CCS String::type() const {
    return "String";
}