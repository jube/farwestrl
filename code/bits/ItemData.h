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
    Firearm,
    Ammunition,
    MeleeWeapon,
  };

  struct FirearmDataFeature {
    int8_t caliber;
    int8_t modifier;
    int16_t capacity;
    uint16_t shoot_time;
    uint16_t reload_time;
  };

  struct AmmunitionDataFeature {
    int8_t caliber;
    Attack attack;
  };

  struct MeleeWeaponDataFeature {
    int8_t modifier;
    Attack attack;
    uint16_t use_time;
  };

  using ItemDataFeature = gf::TaggedVariant<ItemType, FirearmDataFeature, AmmunitionDataFeature, MeleeWeaponDataFeature>;

  struct ItemData {
    DataLabel label;
    DisplayData display;
    ItemDataFeature feature;
  };

  void from_json(const nlohmann::json& j, ItemData& data);

}

#endif // FW_ITEM_DATA_H
