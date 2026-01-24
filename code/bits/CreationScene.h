#ifndef FW_GENERATION_SCENE_H
#define FW_GENERATION_SCENE_H

#include <gf2/core/Time.h>
#include <gf2/core/ConsoleScene.h>
#include <gf2/core/Console.h>

namespace fw {
  class FarWest;

  class CreationScene : public gf::ConsoleScene {
  public:
    CreationScene(FarWest* game);

    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
    gf::Time m_time;
    gf::Console m_console;
  };

}

#endif // FW_GENERATION_SCENE_H
