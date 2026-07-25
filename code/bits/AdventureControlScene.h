#ifndef FW_ADVENTURE_CONTROL_SCENE_H
#define FW_ADVENTURE_CONTROL_SCENE_H

#include <optional>

#include <gf2/core/ActionGroup.h>
#include <gf2/core/ActionSettings.h>
#include <gf2/core/ConsoleScene.h>

#include "Date.h"
#include "MapRuntime.h"

namespace fw {
  class FarWest;

  class AdventureControlScene : public gf::ConsoleScene {
  public:
    AdventureControlScene(FarWest* game);

    void process_event(const gf::Event& event) override;
    void handle_actions() override;
    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    static gf::ActionGroupSettings compute_settings();

    void update_reduced_background();

    FarWest* m_game = nullptr;
    gf::ActionGroup m_action_group;

    Date m_last_grid_update = {};
    gf::Array2D<RuntimeMapCell> m_reduced_background;
    std::vector<gf::Vec2I> m_computed_path;
  };

}

#endif // FW_ADVENTURE_CONTROL_SCENE_H
