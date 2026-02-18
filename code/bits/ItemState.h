#ifndef FW_ITEM_STATE_H
#define FW_ITEM_STATE_H

#include <gf2/core/Vec2.h>

#include "DataReference.h"
#include "ItemData.h"

namespace fw {

  struct ItemState {
    DataReference<ItemData> data;
    gf::Vec2I position;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<ItemState, Archive>& state)
  {
    return ar | state.data | state.position;
  }

  struct InventoryItemState {
    DataReference<ItemData> data;
    int16_t count = 0;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<InventoryItemState, Archive>& state)
  {
    return ar | state.data | state.count;
  }

  struct WeaponItemState {
    DataReference<ItemData> data;
    int16_t cartridges = 0;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<WeaponItemState, Archive>& state)
  {
    return ar | state.data | state.cartridges;
  }

}

#endif // FW_ITEM_STATE_H
