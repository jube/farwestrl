#ifndef FW_INVENTORY_CONSOLE_ENTITY_H
#define FW_INVENTORY_CONSOLE_ENTITY_H

#include <gf2/core/Time.h>
#include <gf2/core/ConsoleEntity.h>

#include "InventoryState.h"

namespace fw {
  class FarWest;

  class InventoryConsoleEntity : public gf::ConsoleEntity {
  public:
    InventoryConsoleEntity(FarWest* game);

    void set_inventory(const InventoryState* state);

    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
    const InventoryState* m_state = nullptr;
    gf::Console m_console;
  };

}

#endif // FW_INVENTORY_CONSOLE_ENTITY_H
