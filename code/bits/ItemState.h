#ifndef FW_ITEM_STATE_H
#define FW_ITEM_STATE_H

#include <vector>

#include <gf2/core/Vec2.h>
#include <gf2/core/TypeTraits.h>

#include "DataReference.h"
#include "Dollar.h"
#include "ItemData.h"
#include "Location.h"

namespace fw {

  struct GenericItemState {
    DataReference<ItemData> data;
    int16_t count = 0;

    ItemType type() const { return data ? data->type() : ItemType::None; }
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<GenericItemState, Archive>& state)
  {
    return ar | state.data | state.count;
  }

  using InventoryItemState = GenericItemState;

  struct ContainerComponent {
    std::vector<InventoryItemState> content;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<ContainerComponent, Archive>& component)
  {
    return ar | component.content;
  }

  struct MeleeWeaponComponent {
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, [[maybe_unused]] gf::MaybeConst<MeleeWeaponComponent, Archive>& component)
  {
    return ar;
  }

  struct DistanceWeaponComponent {
    int16_t projectiles = 0;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<DistanceWeaponComponent, Archive>& component)
  {
    return ar | component.projectiles;
  }

  struct ProjectileComponent {
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, [[maybe_unused]] gf::MaybeConst<ProjectileComponent, Archive>& component)
  {
    return ar;
  }

  struct KeyComponent {
    std::vector<Location> doors;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<KeyComponent, Archive>& component)
  {
    return ar | component.doors;
  }

  using ItemComponent = gf::TaggedVariant<ItemType,
    ContainerComponent,
    MeleeWeaponComponent,
    DistanceWeaponComponent,
    ProjectileComponent,
    KeyComponent
  >;

  struct ItemState {
    DataReference<ItemData> data;
    ItemComponent component;

    ItemType type() const { return component.type(); }
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<ItemState, Archive>& state)
  {
    return ar | state.data | state.component;
  }

  struct LocatedItemState {
    ItemState item;
    Location location;

    ItemType type() const { return item.type(); }
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<LocatedItemState, Archive>& state)
  {
    return ar | state.item | state.location;
  }

  using EquippedItemState = ItemState;
  using UniqueItemState = ItemState;


  struct ShopUniqueItemState {
    UniqueItemState item;
    Dollar price;
  };

  struct ShopGenericItemState {
    GenericItemState item;
    Dollar price;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<ShopGenericItemState, Archive>& state)
  {
    return ar | state.item | state.price;
  }

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<ShopUniqueItemState, Archive>& state)
  {
    return ar | state.item | state.price;
  }

}

#endif // FW_ITEM_STATE_H
