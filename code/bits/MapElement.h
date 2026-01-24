#ifndef FW_MAP_SCENE_H
#define FW_MAP_SCENE_H

#include <gf2/core/ConsoleElement.h>

namespace fw {
  class FarWest;

  class MapElement : public gf::ConsoleElement {
  public:
    MapElement(FarWest* game);

    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
  };

}

#endif // FW_MAP_SCENE_H
