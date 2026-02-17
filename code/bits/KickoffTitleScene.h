#ifndef FW_KICKOFF_TITLE_SCENE_H
#define FW_KICKOFF_TITLE_SCENE_H

#include <gf2/core/Console.h>
#include <gf2/core/ConsoleScene.h>
#include <gf2/core/FontFace.h>

namespace fw {
  class FarWest;

  class KickoffTitleScene : public gf::ConsoleScene {
  public:
    KickoffTitleScene(FarWest* game);

    void render(gf::Console& buffer) override;

  private:
    FarWest* m_game = nullptr;
    gf::Console m_title;
  };

}

#endif // FW_KICKOFF_TITLE_SCENE_H
