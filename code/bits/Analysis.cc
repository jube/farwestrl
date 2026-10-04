#include "Analysis.h"

#include <cassert>

#include <gf2/core/Log.h>

namespace fw {

  namespace {

    std::string_view to_string(AnalysisStep step)
    {
      switch (step) {
        case AnalysisStep::Start:
          return "Start";
        case AnalysisStep::File:
          return "File";
        case AnalysisStep::Load:
          return "Load";
        case AnalysisStep::Date:
          return "Date";
        case AnalysisStep::Terrain:
          return "Terrain";
        case AnalysisStep::Rivers:
          return "Rivers";
        case AnalysisStep::Biomes:
          return "Biomes";
        case AnalysisStep::Moutains:
          return "Mountains";
        case AnalysisStep::Towns:
          return "Towns";
        case AnalysisStep::Rails:
          return "Rails";
        case AnalysisStep::Roads:
          return "Roads";
        case AnalysisStep::Buildings:
          return "Buildings";
        case AnalysisStep::Regions:
          return "Regions";
        case AnalysisStep::Underground:
          return "Underground";
        case AnalysisStep::Hero:
          return "Hero";
        case AnalysisStep::Actors:
          return "Actors";
        case AnalysisStep::Data:
          return "Data";
        case AnalysisStep::MapGround:
          return "MapGround";
        case AnalysisStep::MapUnderground:
          return "MapUnderground";
        case AnalysisStep::MapRails:
          return "MapRails";
        case AnalysisStep::MapRoads:
          return "MapRoads";
        case AnalysisStep::MapTowns:
          return "MapTowns";
        case AnalysisStep::MapBuildings:
          return "MapBuildings";
        case AnalysisStep::MapMinimap:
          return "MapMinimap";
        case AnalysisStep::Network:
          return "Network";
        case AnalysisStep::FirstTurn:
          return "FirstTurn";
        case AnalysisStep::End:
          return "End";
      }

      return "???";
    }

  }

  void Analysis::set_step(AnalysisStep step)
  {
    const AnalysisStep previous = m_current_step.exchange(step);

    if (step == AnalysisStep::Start) {
      m_current_clock.restart();
    } else {
      std::size_t index = static_cast<std::size_t>(previous);
      assert(index < m_step_times.size());
      m_step_times[index] = m_current_clock.restart();
    }
  }

  AnalysisStep Analysis::step() const
  {
    return m_current_step.load();
  }

  void Analysis::print_analysis() const
  {
    for (std::size_t i = 0; i < m_step_times.size(); ++i) {
      const AnalysisStep step = static_cast<AnalysisStep>(i);
      gf::Log::info("{:20} | {:10}", to_string(step), m_step_times[i].as_milliseconds());
    }
  }

}
