#ifndef FW_SHOP_STATE_H
#define FW_SHOP_STATE_H

#include <vector>

#include "ItemState.h"
#include "Location.h"

namespace fw {

  struct ShopState {
    std::vector<ShopGenericItemState> items;
    std::vector<ShopUniqueItemState> unique_items;
    std::vector<Location> showroom;
  };

}


#endif // FW_SHOP_STATE_H
