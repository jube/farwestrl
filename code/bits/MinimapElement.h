#ifndef FW_MINIMAP_ELEMENT_H
#define FW_MINIMAP_ELEMENT_H

#include <gf2/core/ConsoleElement.h>

namespace fw {
  class FarWest;

  class MinimapElement : public gf::ConsoleElement {
  public:
    MinimapElement(FarWest* game);

    void zoom_in();
    void zoom_out();

    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
    std::size_t m_zoom_level = 0;
  };

}

#endif // FW_MINIMAP_ELEMENT_H
