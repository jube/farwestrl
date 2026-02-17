#include "MinimapConsoleEntity.h"

#include <gf2/core/ConsoleOperations.h>

#include "FarWest.h"
#include "MapRuntime.h"
#include "Settings.h"

namespace fw {

  MinimapConsoleEntity::MinimapConsoleEntity(FarWest* game)
  : m_game(game)
  {
  }

  void MinimapConsoleEntity::zoom_in()
  {
    if (m_zoom_level > 0) {
      --m_zoom_level;
    }
  }

  void MinimapConsoleEntity::zoom_out()
  {
    if (m_zoom_level < MinimapCount - 1) {
      ++m_zoom_level;
    }
  }

  void MinimapConsoleEntity::render(gf::Console& console)
  {
    const FloorMap& floor = m_game->runtime()->map.from_floor(m_game->state()->hero().floor);
    const Minimap& minimap = floor.minimaps[m_zoom_level];
    gf::Console minimap_console = minimap.console;

    const gf::Vec2I hero_position = m_game->state()->hero().position / minimap.factor;

    gf::ConsoleStyle hero_style;
    hero_style.color.foreground = gf::Black;
    hero_style.color.background = gf::Transparent;
    hero_style.effect = gf::ConsoleEffect::none();

    gf::console_write_picture(minimap_console, hero_position, u'@', hero_style);

    for (const gf::Vec2I position : minimap.explored.position_range()) {
      gf::console_write_background(minimap_console, position, gf::gray(minimap.explored(position)), gf::ConsoleEffect::multiply());
    }

    const int32_t min_extent = std::min(ConsoleSize.x, ConsoleSize.y);
    const gf::RectI hero_box = gf::RectI::from_center_size(hero_position, { min_extent, min_extent });

    gf::console_blit_to(minimap_console, console, hero_box, (ConsoleSize - min_extent) / 2);
  }

}
