#include "AdventureInventoryScene.h"

#include <gf2/core/ConsoleOperations.h>

#include "FarWest.h"

namespace fw {

  namespace {

  }

  AdventureInventoryScene::AdventureInventoryScene(FarWest* game)
  : m_game(game)
  , m_action_group(compute_settings())
  , m_inventory(game)
  {
    add_entity(&m_inventory);
  }

  void AdventureInventoryScene::process_event(const gf::Event& event)
  {
    m_action_group.process_event(event);
  }

  void AdventureInventoryScene::handle_actions()
  {
    using namespace gf::literals;

    if (m_action_group.active("back"_id)) {
      m_game->replace_scene(&m_game->adventure_control);
    }

    m_action_group.reset();
  }

  void AdventureInventoryScene::update(gf::Time time)
  {
    update_inventory();
    update_entities(time);
  }

  void AdventureInventoryScene::render(gf::Console& console)
  {
    update_inventory();
    render_entities(console);
  }

  gf::ActionGroupSettings AdventureInventoryScene::compute_settings()
  {
    using namespace gf::literals;
    gf::ActionGroupSettings settings;

    settings.actions.emplace("back"_id, gf::instantaneous_action().add_keycode_control(gf::Keycode::I));

    return settings;
  }

  void AdventureInventoryScene::update_inventory()
  {
    m_inventory.set_inventory(&m_game->state()->hero().feature.from<ActorType::Human>().inventory);
  }

}
