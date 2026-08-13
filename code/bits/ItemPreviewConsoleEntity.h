#ifndef FW_ITEM_PREVIEW_CONSOLE_ENTITY_H
#define FW_ITEM_PREVIEW_CONSOLE_ENTITY_H

#include <gf2/core/Time.h>
#include <gf2/core/ConsoleEntity.h>

#include "ItemData.h"

namespace fw {
  class FarWest;

  constexpr gf::Vec2I ItemPreviewConsoleSize = { 29, 43 };

  class ItemPreviewConsoleEntity : public gf::ConsoleEntity {
  public:
    ItemPreviewConsoleEntity(FarWest* game);

    void set_item(const ItemData* data);

    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
    const ItemData* m_data = nullptr;
  };

}

#endif // FW_ITEM_PREVIEW_CONSOLE_ENTITY_H
