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

  Place generate_town(gf::Random* random);

  enum class LocalityType : uint8_t {
    Farm,
    Camp, // cavalry camp
    Village, // native vilage
  };

  Place generate_locality(LocalityType type, gf::Random* random);

}

#endif // FW_WORLD_GENERATION_PLACES_H
