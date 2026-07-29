#ifndef FW_ITEM_DATA_H
#define FW_ITEM_DATA_H

#include <cstdint>

#include <nlohmann/json.hpp>

#include <gf2/core/Color.h>
#include <gf2/core/TaggedVariant.h>

#include "Combat.h"
#include "DisplayData.h"
#include "DataLabel.h"

namespace fw {

  enum class ItemType {
    None,
    MeleeWeapon,
    DistanceWeapon,
    Ammunition,
  };

  struct DistanceWeaponDataFeature {
    int32_t range;
    uint16_t shoot_time;
    uint16_t reload_time;
    int16_t capacity;
    int8_t caliber;
    int8_t modifier;
  };

  struct AmmunitionDataFeature {
    Attack attack;
    int8_t caliber;
    int8_t modifier;
  };

  struct MeleeWeaponDataFeature {
    Attack attack;
    uint16_t use_time;
    int8_t modifier;
  };

  using ItemDataFeature = gf::TaggedVariant<ItemType, MeleeWeaponDataFeature, DistanceWeaponDataFeature, AmmunitionDataFeature>;

  struct ItemData {
    DataLabel label;
    DisplayData display;
    ItemDataFeature feature;
  };

  void from_json(const nlohmann::json& j, ItemData& data);

}

#endif // FW_ITEM_DATA_H
