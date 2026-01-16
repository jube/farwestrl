#ifndef FFW_WORLD_GENERATION_STEP_H
#define FFW_WORLD_GENERATION_STEP_H

#include <cstdint>

namespace ffw {

  enum class WorldGenerationStep : uint8_t {
    Start,
    File,
    // loading
    Load,
    // generation
    Date,
    Terrain,
    Biomes,
    Moutains,
    Towns,
    Rails,
    Roads,
    Buildings,
    Regions,
    Underground,
    Hero,
    Actors,
    // binding
    Data,
    MapGround,
    MapUnderground,
    MapRails,
    MapRoads,
    MapTowns,
    MapBuildings,
    MapMinimap,
    Network,

    End,
  };

}

#endif // FFW_WORLD_GENERATION_STEP_H
