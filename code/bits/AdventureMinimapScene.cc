#include "AdventureMinimapScene.h"

#include "FarWest.h"

namespace fw {

  AdventureMinimapScene::AdventureMinimapScene(FarWest* game)
  : m_game(game)
  , m_action_group(compute_settings())
  , m_minimap(game)
  {
    add_entity(&m_minimap);
  }

  void AdventureMinimapScene::process_event(const gf::Event& event)
  {
    m_action_group.process_event(event);
  }

  void AdventureMinimapScene::handle_actions()
  {
    using namespace gf::literals;

    if (m_action_group.active("zoom_in"_id)) {
      m_minimap.zoom_in();
    }

    if (m_action_group.active("zoom_out"_id)) {
      m_minimap.zoom_out();
    }

    if (m_action_group.active("back"_id)) {
      m_game->start_adventure();
    }

    m_action_group.reset();
  }

  gf::ActionGroupSettings AdventureMinimapScene::compute_settings()
  {
    using namespace gf::literals;
    gf::ActionGroupSettings settings;

    settings.actions.emplace("zoom_in"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::NumpadPlus).add_scancode_control(gf::Scancode::F11));
    settings.actions.emplace("zoom_out"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::NumpadMinus).add_scancode_control(gf::Scancode::F12));
    settings.actions.emplace("back"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::Tab));

    return settings;
  }

}
