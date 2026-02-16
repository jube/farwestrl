#ifndef FW_FAR_FAR_WEST_SCENE_H
#define FW_FAR_FAR_WEST_SCENE_H

#include <gf2/core/ActionSettings.h>
#include <gf2/core/ActionGroup.h>
#include <gf2/graphics/ConsoleGraphicsEntity.h>
#include <gf2/graphics/Scene.h>

#include "FarWest.h"
#include "FarWestResources.h"

namespace fw {
  class FarWestSystem;

  class FarWestScene : public gf::Scene {
  public:
    FarWestScene(FarWestSystem* game, const FarWestResources& resources);

  private:
    static gf::ActionGroupSettings compute_settings();

    void do_process_event(const gf::Event& event) override;
    void do_handle_actions() override;
    void do_update(gf::Time time) override;

    FarWestSystem* m_game;
    gf::ActionGroup m_action_group;
    FarWest m_console_scene_manager;
    gf::ConsoleGraphicsEntity m_console_entity;
  };

}

#endif // FW_FAR_FAR_WEST_SCENE_H
