#ifndef FW_ANALYSIS_H
#define FW_ANALYSIS_H

#include <cstdint>

#include <array>
#include <atomic>

#include <gf2/core/Clock.h>
#include <gf2/core/Time.h>

namespace fw {

  enum class AnalysisStep : uint8_t {
    Start,
    File,
    // loading
    Load,
    // generation
    Date,
    Terrain,
    Rivers,
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

    FirstTurn,
    End,
  };

  class Analysis {
  public:

    void set_step(AnalysisStep step);
    AnalysisStep step() const;

    void print_analysis() const;

  private:
    static constexpr std::size_t StepCount = static_cast<std::size_t>(AnalysisStep::End);
    gf::Clock m_current_clock;
    std::atomic<AnalysisStep> m_current_step = AnalysisStep::Start;
    std::array<gf::Time, StepCount> m_step_times = {};
  };


}

#endif // FW_ANALYSIS_H
