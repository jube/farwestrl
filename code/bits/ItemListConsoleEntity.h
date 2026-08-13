#ifndef FW_ITEM_LIST_CONSOLE_ENTITY_H
#define FW_ITEM_LIST_CONSOLE_ENTITY_H

#include <gf2/core/Time.h>
#include <gf2/core/ConsoleEntity.h>

#include "InventoryState.h"

namespace fw {
  class FarWest;

  constexpr gf::Vec2I ItemListConsoleSize = { 29, 43 };

  class ItemListConsoleEntity : public gf::ConsoleEntity {
  public:
    ItemListConsoleEntity(FarWest* game);

    void set_inventory(const InventoryState* state);
    const ItemData* current_item() const;

    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
    const InventoryState* m_state = nullptr;
  };

}

#endif // FW_ITEM_LIST_CONSOLE_ENTITY_H
