#include "SaveScene.h"

#include <gf2/core/ConsoleOperations.h>

#include "Colors.h"
#include "FarWest.h"

namespace fw {

  namespace {

    constexpr gf::Vec2I SaveConsoleSize = { 23, 3 };
    constexpr float DotsPerSeconds = 1.5f;

  }

  SaveScene::SaveScene(FarWest* game)
  : m_game(game)
  , m_console(SaveConsoleSize)
  {
  }

  void SaveScene::update(gf::Time time)
  {
    m_time += time;

    if (m_game->save_creation_finished()) {
      m_game->pop_all_scenes();
    }
  }

  void SaveScene::render(gf::Console& console)
  {
    gf::ConsoleStyle style;
    style.color.background = RpgBlue;
    style.color.foreground = gf::White;
    style.effect = gf::ConsoleEffect::set();

    gf::console_clear(m_console, style);
    gf::console_draw_frame(m_console, gf::RectI::from_size(SaveConsoleSize), style);

    const std::size_t dots = std::size_t(m_time.as_seconds() * DotsPerSeconds) % 4;
    gf::console_print_text(m_console, { 2, 1 }, gf::ConsoleAlignment::Left, style, "{}", "Saving the adventure before leaving" + std::string(dots, '.'));

    const gf::Vec2I padding = console.size() - m_console.size();
    const gf::Vec2I save_position = padding / 2;
    gf::console_blit_to(m_console, console, save_position, 1.0f, RpgBlueAlpha);
  }

}
