#include "WorldGenerationStep.h"

#include <cassert>

#include <gf2/core/Log.h>

namespace fw {

  namespace {

    std::string_view to_string(WorldGenerationStep step)
    {
      switch (step) {
        case WorldGenerationStep::Start:
          return "Start";
        case WorldGenerationStep::File:
          return "File";
        case WorldGenerationStep::Load:
          return "Load";
        case WorldGenerationStep::Date:
          return "Date";
        case WorldGenerationStep::Terrain:
          return "Terrain";
        case WorldGenerationStep::Biomes:
          return "Biomes";
        case WorldGenerationStep::Moutains:
          return "Mountains";
        case WorldGenerationStep::Towns:
          return "Towns";
        case WorldGenerationStep::Rails:
          return "Rails";
        case WorldGenerationStep::Roads:
          return "Roads";
        case WorldGenerationStep::Buildings:
          return "Buildings";
        case WorldGenerationStep::Regions:
          return "Regions";
        case WorldGenerationStep::Underground:
          return "Underground";
        case WorldGenerationStep::Hero:
          return "Hero";
        case WorldGenerationStep::Actors:
          return "Actors";
        case WorldGenerationStep::Data:
          return "Data";
        case WorldGenerationStep::MapGround:
          return "MapGround";
        case WorldGenerationStep::MapUnderground:
          return "MapUnderground";
        case WorldGenerationStep::MapRails:
          return "MapRails";
        case WorldGenerationStep::MapRoads:
          return "MapRoads";
        case WorldGenerationStep::MapTowns:
          return "MapTowns";
        case WorldGenerationStep::MapBuildings:
          return "MapBuildings";
        case WorldGenerationStep::MapMinimap:
          return "MapMinimap";
        case WorldGenerationStep::Network:
          return "Network";
        case WorldGenerationStep::End:
          return "End";
      }

      return "???";
    }

  }

  void WorldGenerationAnalysis::set_step(WorldGenerationStep step)
  {
    const WorldGenerationStep previous = m_current_step.exchange(step);

    if (step == WorldGenerationStep::Start) {
      m_current_clock.restart();
    } else {
      std::size_t index = static_cast<std::size_t>(previous);
      assert(index <m_step_times.size());
      m_step_times[index] = m_current_clock.restart();
    }
  }

  WorldGenerationStep WorldGenerationAnalysis::step() const
  {
    return m_current_step.load();
  }

  void WorldGenerationAnalysis::print_analysis() const
  {
    for (std::size_t i = 0; i < m_step_times.size(); ++i) {
      const WorldGenerationStep step = static_cast<WorldGenerationStep>(i);
      gf::Log::info("{:20} | {:10}", to_string(step), m_step_times[i].as_milliseconds());
    }
  }

}
