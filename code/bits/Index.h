#ifndef FW_INDEX_H
#define FW_INDEX_H

#include <cstdint>

#include <limits>

namespace fw {

  constexpr uint32_t NoIndex = std::numeric_limits<uint32_t>::max();
  constexpr uint32_t HeroIndex = 0;

}

#endif // FW_INDEX_H
