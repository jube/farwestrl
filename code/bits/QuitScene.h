#ifndef FW_QUIT_SCENE_H
#define FW_QUIT_SCENE_H

#include <gf2/core/ActionGroup.h>
#include <gf2/core/ActionSettings.h>
#include <gf2/core/ConsoleScene.h>
#include <gf2/core/Console.h>

namespace fw {
  class FarWest;

  class QuitScene : public gf::ConsoleScene {
  public:
    QuitScene(FarWest* game);

    void process_event(const gf::Event& event) override;
    void handle_actions() override;
    void render(gf::Console& console) override;

  private:
    static gf::ActionGroupSettings compute_settings();

    FarWest* m_game = nullptr;
    gf::ActionGroup m_action_group;
    gf::Console m_console;
    int m_choice = 0;
  };

}

#endif // FW_QUIT_SCENE_H
