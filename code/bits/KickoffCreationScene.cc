#include "KickoffCreationScene.h"

#include "Analysis.h"
#include "FarWest.h"

#include <gf2/core/Color.h>
#include <gf2/core/ConsoleOperations.h>

namespace fw {

  namespace {

    constexpr gf::Vec2I CreationConsoleSize = { 17, 4 };
    constexpr float DotsPerSeconds = 1.5f;

    std::string_view compute_step(AnalysisStep step)
    {
      switch (step) {
        case AnalysisStep::Start:
          return "Let's go!";
        case AnalysisStep::File:
          return "Loading data file";
        case AnalysisStep::Load:
          return "Loading save file";
        case AnalysisStep::Date:
          return "Choosing a date";
        case AnalysisStep::Terrain:
          return "Generating the terrain";
        case AnalysisStep::Rivers:
          return "Digging the rivers";
        case AnalysisStep::Biomes:
          return "Determining the biomes";
        case AnalysisStep::Moutains:
          return "Creating some moutains";
        case AnalysisStep::Towns:
          return "Determining town locations";
        case AnalysisStep::Rails:
          return "Constructing railways";
        case AnalysisStep::Roads:
          return "Making roads";
        case AnalysisStep::Buildings:
          return "Placing buildings in towns";
        case AnalysisStep::Regions:
          return "Listing regions";
        case AnalysisStep::Underground:
          return "Excavating underground";
        case AnalysisStep::Hero:
          return "Raising the hero";
        case AnalysisStep::Actors:
          return "Spawning the actors";
        case AnalysisStep::Data:
          return "Linking to data";
        case AnalysisStep::MapGround:
          return "Setting the map ground";
        case AnalysisStep::MapUnderground:
          return "Setting the map underground";
        case AnalysisStep::MapRails:
          return "Installing rails";
        case AnalysisStep::MapRoads:
          return "Linking roads";
        case AnalysisStep::MapTowns:
          return "Constructing the buildings";
        case AnalysisStep::MapBuildings:
          return "Elevating buildings";
        case AnalysisStep::MapMinimap:
          return "Generating the minimaps";
        case AnalysisStep::Network:
          return "Tracing the railway network";
        case AnalysisStep::FirstTurn:
          return "Starting the world";
        case AnalysisStep::End:
          return "The end!";
      }

      return "???";
    }

  }

  KickoffCreationScene::KickoffCreationScene(FarWest* game)
  : m_game(game)
  , m_console(CreationConsoleSize)
  {
  }

  void KickoffCreationScene::update(gf::Time time)
  {
    m_time += time;

    if (m_game->world_creation_finished()) {
      m_game->start_adventure();
    }
  }

  void KickoffCreationScene::render(gf::Console& console)
  {
    gf::ConsoleStyle style;
    style.color.background = gf::Black;
    style.color.foreground = gf::White;
    style.effect = gf::ConsoleEffect::set();

    gf::console_clear(m_console, style);
    gf::console_draw_frame(m_console, gf::RectI::from_size(CreationConsoleSize), style);

    const std::size_t dots = std::size_t(m_time.as_seconds() * DotsPerSeconds) % 4;
    gf::console_print_text(m_console, { 3, 1 }, gf::ConsoleAlignment::Left, style, "{}", "Creation of the world" + std::string(dots, '.'));

    style.color.foreground = gf::Amber;
    const AnalysisStep step = m_game->world_creation_step();
    const std::string_view step_name = compute_step(step);
    gf::console_print_text(m_console, { CreationConsoleSize.x / 2, 2 }, gf::ConsoleAlignment::Center, style, "{}", step_name);

    const gf::Vec2I padding = console.size() - m_console.size();
    const gf::Vec2I creation_position = { padding.x / 2, padding.y / 2 + 10 };
    gf::console_blit_to(m_console, console, creation_position);
  }

}
