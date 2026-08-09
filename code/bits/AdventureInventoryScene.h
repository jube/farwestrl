#ifndef FW_ADVENTURE_INVENTORY_SCENE_H
#define FW_ADVENTURE_INVENTORY_SCENE_H

#include <gf2/core/ActionGroup.h>
#include <gf2/core/ActionSettings.h>
#include <gf2/core/Console.h>
#include <gf2/core/ConsoleScene.h>

#include "InventoryConsoleEntity.h"

namespace fw {
  class FarWest;

  class AdventureInventoryScene : public gf::ConsoleScene {
  public:
    AdventureInventoryScene(FarWest* game);

    void process_event(const gf::Event& event) override;
    void handle_actions() override;
    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    static gf::ActionGroupSettings compute_settings();

    void update_inventory();

    FarWest* m_game = nullptr;
    gf::ActionGroup m_action_group;

    InventoryConsoleEntity m_inventory;
  };

}

#endif // FW_ADVENTURE_INVENTORY_SCENE_H
