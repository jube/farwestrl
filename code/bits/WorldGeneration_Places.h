#ifndef FW_WORLD_GENERATION_PLACES_H
#define FW_WORLD_GENERATION_PLACES_H

#include <gf2/core/Array2D.h>
#include <gf2/core/Random.h>

#include "DoorState.h"
#include "MapCell.h"
#include "ThingState.h"

namespace fw {

  struct Place {
    Place(int32_t size);

    gf::Array2D<MapCell> map;
    std::vector<ThingState> things;
    std::vector<DoorState> doors;
  };

  constexpr std::size_t TownsCount = 5;

  constexpr int32_t TownsBlockSize = 6;
  constexpr int32_t TownBuildingSize = 11;
  constexpr int32_t StreetSize = 3;

  constexpr int32_t TownRadius = (TownsBlockSize * TownBuildingSize + (TownsBlockSize - 1) * StreetSize - 1) / 2;
  constexpr int32_t TownDiameter = 2 * TownRadius + 1;

  Place generate_town(gf::Random* random);

  constexpr std::size_t LocalityPerTown = 5;
  constexpr std::size_t LocalityCount = TownsCount * LocalityPerTown;

  constexpr int32_t LocalityRadius = 13;
  constexpr int32_t LocalityDiameter = 2 * LocalityRadius + 1;

  enum class LocalityType : uint8_t {
    Farm,
    Camp, // cavalry camp
    Village, // native vilage
  };

  Place generate_locality(LocalityType type, gf::Random* random);

}

#endif // FW_WORLD_GENERATION_PLACES_H
