#ifndef FW_MAP_CELL_BIOME_H
#define FW_MAP_CELL_BIOME_H

#include <cstddef>
#include <cstdint>

namespace fw {

  enum class MapCellBiome : uint8_t {
    None,

    Prairie,
    Desert,
    Forest,
    Mountain,

    Underground,
    Building,
  };

  inline constexpr std::size_t MapCellBiomeCount = 7;

}

#endif // FW_MAP_CELL_BIOME_H
