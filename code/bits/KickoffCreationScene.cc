#include "KickoffCreationScene.h"

#include "FarWest.h"
#include "WorldGenerationStep.h"

#include <gf2/core/Color.h>
#include <gf2/core/ConsoleOperations.h>

namespace fw {

  namespace {

    constexpr gf::Vec2I CreationConsoleSize = { 17, 4 };
    constexpr float DotsPerSeconds = 1.5f;

    std::string_view compute_step(WorldGenerationStep step)
    {
      switch (step) {
        case WorldGenerationStep::Start:
          return "Let's go!";
        case WorldGenerationStep::File:
          return "Loading data file";
        case WorldGenerationStep::Load:
          return "Loading save file";
        case WorldGenerationStep::Date:
          return "Choosing a date";
        case WorldGenerationStep::Terrain:
          return "Generating the terrain";
        case WorldGenerationStep::Biomes:
          return "Determining the biomes";
        case WorldGenerationStep::Moutains:
          return "Creating some moutains";
        case WorldGenerationStep::Towns:
          return "Determining town locations";
        case WorldGenerationStep::Rails:
          return "Consructing railways";
        case WorldGenerationStep::Roads:
          return "Making roads";
        case WorldGenerationStep::Buildings:
          return "Placing buildings in towns";
        case WorldGenerationStep::Regions:
          return "Listing regions";
        case WorldGenerationStep::Underground:
          return "Digging underground";
        case WorldGenerationStep::Hero:
          return "Raising the hero";
        case WorldGenerationStep::Actors:
          return "Spawning the actors";
        case WorldGenerationStep::Data:
          return "Linking to data";
        case WorldGenerationStep::MapGround:
          return "Setting the map ground";
        case WorldGenerationStep::MapUnderground:
          return "Setting the map underground";
        case WorldGenerationStep::MapRails:
          return "Installing rails";
        case WorldGenerationStep::MapRoads:
          return "Linking roads";
        case WorldGenerationStep::MapTowns:
          return "Constructing the buildings";
        case WorldGenerationStep::MapBuildings:
          return "Elevating buildings";
        case WorldGenerationStep::MapMinimap:
          return "Generating the minimaps";
        case WorldGenerationStep::Network:
          return "Tracing the railway network";
        case WorldGenerationStep::End:
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
    const WorldGenerationStep step = m_game->world_creation_step();
    const std::string_view step_name = compute_step(step);
    gf::console_print_text(m_console, { CreationConsoleSize.x / 2, 2 }, gf::ConsoleAlignment::Center, style, "{}", step_name);

    const gf::Vec2I padding = console.size() - m_console.size();
    const gf::Vec2I creation_position = { padding.x / 2, padding.y / 2 + 10 };
    gf::console_blit_to(m_console, console, creation_position);
  }

}
