#ifndef FW_ADVENTURE_SAVE_SCENE_H
#define FW_ADVENTURE_SAVE_SCENE_H

#include <gf2/core/ConsoleScene.h>
#include <gf2/core/Console.h>

namespace fw {
  class FarWest;

  class AdventureSaveScene : public gf::ConsoleScene {
  public:
    AdventureSaveScene(FarWest* game);

    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
    gf::Time m_time;
    gf::Console m_console;
  };

}

#endif // FW_ADVENTURE_SAVE_SCENE_H
