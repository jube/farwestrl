#include "KickoffMenuScene.h"

#include <gf2/core/ConsoleOperations.h>

#include "FarWest.h"

namespace fw {

  namespace {
    constexpr int ChoiceCount = 3;
    constexpr int StartNewGameChoice = 0;
    constexpr int ContinueGameChoice = 1;
    constexpr int QuitChoice = 2;

  }

  KickoffMenuScene::KickoffMenuScene(FarWest* game)
  : m_game(game)
  , m_action_group(compute_settings())
  {
    // TODO: cannot do this for now as the order of initialization in FarWest is not OK
    // if (m_game->has_save()) {
    //   m_choice = ContinueGameChoice;
    // }
  }

  gf::ActionGroupSettings KickoffMenuScene::compute_settings()
  {
    using namespace gf::literals;
    gf::ActionGroupSettings settings;

    settings.actions.emplace("down"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::Down));
    settings.actions.emplace("up"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::Up));
    settings.actions.emplace("choose"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::Space));

    return settings;
  }

  void KickoffMenuScene::process_event(const gf::Event& event)
  {
    m_action_group.process_event(event);
  }

  void KickoffMenuScene::handle_actions()
  {
    using namespace gf::literals;

    if (m_action_group.active("down"_id)) {
      m_choice = (m_choice + 1) % ChoiceCount;
    } else if (m_action_group.active("up"_id)) {
      m_choice = (m_choice + ChoiceCount- 1) % ChoiceCount;
    } else if (m_action_group.active("choose"_id)) {
      switch (m_choice) {
        case StartNewGameChoice:
          m_game->create_world(AdventureChoice::New);
          m_game->replace_scene(&m_game->kickoff_creation);
          break;
        case ContinueGameChoice:
          if (m_game->has_save()) {
            m_game->create_world(AdventureChoice::Saved);
            m_game->replace_scene(&m_game->kickoff_creation);
          }
          break;
        case QuitChoice:
          m_game->pop_all_scenes();
          break;
      }
    }

    m_action_group.reset();
  }

  void KickoffMenuScene::render(gf::Console& console)
  {
    const int32_t offset = 40;

    gf::ConsoleStyle style;

    style.color.foreground = gf::White;
    gf::console_print_text(console, { offset, 35 }, gf::ConsoleAlignment::Left, style, "Start a new adventure");

    if (m_game->has_save()) {
      style.color.foreground = gf::White;
    } else {
      style.color.foreground = gf::Gray;
    }

    gf::console_print_text(console, { offset, 36 }, gf::ConsoleAlignment::Left, style, "Continue the previous adventure");

    style.color.foreground = gf::White;
    gf::console_print_text(console, { offset, 37 }, gf::ConsoleAlignment::Left, style, "Quit");

    style.color.foreground = gf::White;
    gf::console_print_text(console, { offset - 1, 35 + m_choice }, gf::ConsoleAlignment::Left, style, ">");
  }

}
