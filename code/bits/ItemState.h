#ifndef FW_ITEM_STATE_H
#define FW_ITEM_STATE_H

#include <vector>

#include <gf2/core/Vec2.h>

#include "DataReference.h"
#include "ItemData.h"
#include "Location.h"

namespace fw {

  struct ContainerFeature {
    std::vector<DataReference<ItemData>> content;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<ContainerFeature, Archive>& feature)
  {
    return ar | feature.content;
  }

  struct MeleeWeaponFeature {
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, [[maybe_unused]] gf::MaybeConst<MeleeWeaponFeature, Archive>& feature)
  {
    return ar;
  }

  struct DistanceWeaponFeature {
    int16_t projectiles = 0;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<DistanceWeaponFeature, Archive>& feature)
  {
    return ar | feature.projectiles;
  }

  struct ProjectileFeature {
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, [[maybe_unused]] gf::MaybeConst<ProjectileFeature, Archive>& feature)
  {
    return ar;
  }

  using ItemFeature = gf::TaggedVariant<ItemType, ContainerFeature, MeleeWeaponFeature, DistanceWeaponFeature, ProjectileFeature>;

  struct ItemState {
    DataReference<ItemData> data;
    ItemFeature feature;
    Location location;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<ItemState, Archive>& state)
  {
    return ar | state.data | state.feature | state.location;
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
    ItemFeature feature;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<WeaponItemState, Archive>& state)
  {
    return ar | state.data | state.feature;
  }

}

#endif // FW_ITEM_STATE_H
