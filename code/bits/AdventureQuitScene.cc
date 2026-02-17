#include "AdventureQuitScene.h"

#include <gf2/core/ConsoleOperations.h>

#include "Colors.h"
#include "FarWest.h"

namespace fw {

  namespace {

    constexpr int ChoiceCount = 3;
    constexpr int SaveAndQuitChoice = 0;
    constexpr int QuitChoice = 1;
    constexpr int ReturnChoice = 2;

    constexpr gf::Vec2I QuitConsoleSize = { 20, 5 };

  }

  AdventureQuitScene::AdventureQuitScene(FarWest* game)
  : m_game(game)
  , m_action_group(compute_settings())
  , m_console(QuitConsoleSize)
  {
  }

  void AdventureQuitScene::process_event(const gf::Event& event)
  {
    m_action_group.process_event(event);
  }

  void AdventureQuitScene::handle_actions()
  {
    using namespace gf::literals;

    if (m_action_group.active("down"_id)) {
      m_choice = (m_choice + 1) % ChoiceCount;
    } else if (m_action_group.active("up"_id)) {
      m_choice = (m_choice + ChoiceCount- 1) % ChoiceCount;
    } else if (m_action_group.active("choose"_id)) {
      switch (m_choice) {
        case SaveAndQuitChoice:
          m_game->create_save();
          m_game->replace_scene(&m_game->adventure_save);
          break;
        case QuitChoice:
          m_game->pop_all_scenes();
          break;
        case ReturnChoice:
          m_game->replace_scene(&m_game->adventure_control);
          break;
      }
    } else if (m_action_group.active("escape"_id)) {
      m_game->replace_scene(&m_game->adventure_control);
    }

    m_action_group.reset();
  }

  void AdventureQuitScene::render(gf::Console& console)
  {
    gf::ConsoleStyle style;
    style.color.background = RpgBlue;
    style.color.foreground = gf::White;
    style.effect = gf::ConsoleEffect::set();

    gf::console_clear(m_console, style);

    gf::console_draw_frame(m_console, gf::RectI::from_size(QuitConsoleSize), style);

    gf::console_print_text(m_console, { 3, 1 }, gf::ConsoleAlignment::Left, style, "Back to real life, see you soon");
    gf::console_print_text(m_console, { 3, 2 }, gf::ConsoleAlignment::Left, style, "Back to real life, farewell");
    gf::console_print_text(m_console, { 3, 3 }, gf::ConsoleAlignment::Left, style, "Back to adventure");

    gf::console_print_text(m_console, { 2, 1 + m_choice }, gf::ConsoleAlignment::Left, style, ">");

    const gf::Vec2I padding = console.size() - m_console.size();
    const gf::Vec2I quit_position = padding / 2;
    gf::console_blit_to(m_console, console, quit_position, 1.0f, RpgBlueAlpha);
  }

  gf::ActionGroupSettings AdventureQuitScene::compute_settings()
  {
    using namespace gf::literals;
    gf::ActionGroupSettings settings;

    settings.actions.emplace("down"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::Down));
    settings.actions.emplace("up"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::Up));
    settings.actions.emplace("choose"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::Space));
    settings.actions.emplace("escape"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::Escape));

    return settings;
  }

}
