#ifndef FW_MINIMAP_CONSOLE_ENTITY_H
#define FW_MINIMAP_CONSOLE_ENTITY_H

#include <gf2/core/ConsoleEntity.h>

namespace fw {
  class FarWest;

  class MinimapConsoleEntity : public gf::ConsoleEntity {
  public:
    MinimapConsoleEntity(FarWest* game);

    void zoom_in();
    void zoom_out();

    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
    std::size_t m_zoom_level = 0;
  };

}

#endif // FW_MINIMAP_CONSOLE_ENTITY_H
