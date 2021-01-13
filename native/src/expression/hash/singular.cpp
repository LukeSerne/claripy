/** @file */
#include "singular.hpp"

#include "../../annotation.hpp"

#include <memory>
#include <sstream>


// For brevity
using namespace Expression;
template <typename T> using Ret = typename Hash::SingularRetMap<T>::RetType;


// A macro for simplicity
#define SINGULAR(T, N) template <> Ret<T> Hash::singular<T>(T const &N)


/** A specialization for T = char * */
SINGULAR(char *, c) {
    return c;
}

/** A specialization for T = Constants::Int */
SINGULAR(Constants::Int, c) {
    return c;
}

/** A specialization for T = std::vector<std::shared_ptr<Annotation::Base>> */
SINGULAR(std::vector<std::shared_ptr<Annotation::Base>>, v) {
    std::ostringstream s;
    for (Constants::UInt i = 0; i < v.size(); ++i) {
        s << v[i]->hash();
    }
    return s.str();
}