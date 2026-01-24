#ifndef FW_KICKOFF_SCENE_H
#define FW_KICKOFF_SCENE_H

#include <gf2/core/ActionSettings.h>
#include <gf2/core/ActionGroup.h>
#include <gf2/core/ConsoleScene.h>

namespace fw {
  class FarWest;

  class KickoffScene : public gf::ConsoleScene {
  public:
    KickoffScene(FarWest* game);

    void process_event(const gf::Event& event) override;
    void handle_actions() override;
    void render(gf::Console& console) override;

  private:
    static gf::ActionGroupSettings compute_settings();

    FarWest* m_game = nullptr;
    gf::ActionGroup m_action_group;
    int m_choice = 0;
  };

}

#endif // FW_KICKOFF_SCENE_H
