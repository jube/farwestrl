#ifndef FW_HERO_SCENE_H
#define FW_HERO_SCENE_H

#include <gf2/core/Time.h>
#include <gf2/core/ConsoleElement.h>

namespace fw {
  class FarWest;

  class HeroElement : public gf::ConsoleElement {
  public:
    HeroElement(FarWest* game);

    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
  };

}

#endif // FW_HERO_SCENE_H
