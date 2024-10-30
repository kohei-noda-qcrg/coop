#pragma once
#include "cohandle.hpp"

namespace coop {
template <class T>
concept CoGeneratorLike = requires(T generator) {
    requires CoHandleLike<decltype(generator.handle)>;
};

template <class T>
struct CoGenerator;
} // namespace coop
