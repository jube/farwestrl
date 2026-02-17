#ifndef FW_MAP_CONSOLE_ENTITY_H
#define FW_MAP_CONSOLE_ENTITY_H

#include <gf2/core/ConsoleEntity.h>

namespace fw {
  class FarWest;

  class MapConsoleEntity : public gf::ConsoleEntity {
  public:
    MapConsoleEntity(FarWest* game);

    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
  };

}

#endif // FW_MAP_CONSOLE_ENTITY_H
