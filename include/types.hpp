#ifndef SYMULACJASIECI_TYPES_HPP
#define SYMULACJASIECI_TYPES_HPP

#include <cstdint>
#include <functional>

using ElementID = uint64_t;

using Time = int32_t;

using TimeOffset = int32_t;

using ProbabilityGenerator = std::function<double()>;

#endif //SYMULACJASIECI_TYPES_HPP
