#include "ContextualConsoleEntity.h"

#include <gf2/core/ConsoleOperations.h>

#include "FarWest.h"
#include "Settings.h"

namespace fw {

  ContextualConsoleEntity::ContextualConsoleEntity(FarWest* game)
  : m_game(game)
  {
  }

  void ContextualConsoleEntity::update([[maybe_unused]] gf::Time time)
  {
  }

  void ContextualConsoleEntity::render(gf::Console& console)
  {
    [[maybe_unused]] WorldState *state = m_game->state();

    gf::ConsoleStyle contextual_box_style;
    contextual_box_style.color.foreground = gf::Gray;
    gf::console_draw_frame(console, ContextualBox, contextual_box_style);

  }

}
