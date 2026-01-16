#ifndef FFW_WORLD_GENERATION_STEP_H
#define FFW_WORLD_GENERATION_STEP_H

#include <cstdint>

#include <array>

#include <gf2/core/Clock.h>
#include <gf2/core/Time.h>

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

  class WorldGenerationAnalysis {
  public:

    void set_step(WorldGenerationStep step);
    WorldGenerationStep step() const;

    void print_analysis() const;

  private:
    static constexpr std::size_t StepCount = static_cast<std::size_t>(WorldGenerationStep::End);
    std::array<gf::Time, StepCount> m_step_times = {};
  };


}

#endif // FFW_WORLD_GENERATION_STEP_H
