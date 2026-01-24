#ifndef FW_INVENTORY_STATE_H
#define FW_INVENTORY_STATE_H

#include <cstdint>

#include <vector>

#include "ItemState.h"

namespace fw {

  struct InventoryState {
    int32_t cash = 0;
    std::vector<InventoryItemState> items;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<InventoryState, Archive>& state)
  {
    return ar | state.cash | state.items;
  }

}

#endif // FW_INVENTORY_STATE_H
