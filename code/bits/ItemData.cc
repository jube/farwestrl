#include "ItemData.h"

namespace fw {

  NLOHMANN_JSON_SERIALIZE_ENUM( ItemType, {
    { ItemType::None, nullptr },
    { ItemType::Firearm, "firearm" },
    { ItemType::Ammunition, "ammunition" },
    { ItemType::MeleeWeapon, "melee_weapon" },
  })

  void from_json(const nlohmann::json& json, ItemData& data)
  {
    json.at("label").get_to(data.label);
    json.at("display").get_to(data.display);

    ItemType raw_type = ItemType::None;
    json.at("type").get_to(raw_type);

    switch (raw_type) {
      case ItemType::None:
        // nothing
        break;
      case ItemType::Firearm:
        {
          FirearmDataFeature feature;
          json.at("caliber").get_to(feature.caliber);
          json.at("modifier").get_to(feature.modifier);
          json.at("capacity").get_to(feature.capacity);
          json.at("reload_time").get_to(feature.reload_time);
          json.at("shoot_time").get_to(feature.shoot_time);
          data.feature = feature;
        }
        break;
      case ItemType::Ammunition:
        {
          AmmunitionDataFeature feature;
          json.at("caliber").get_to(feature.caliber);
          json.at("attack").get_to(feature.attack);
          data.feature = feature;
        }
        break;
      case ItemType::MeleeWeapon:
        {
          MeleeWeaponDataFeature feature;
          json.at("modifier").get_to(feature.modifier);
          json.at("attack").get_to(feature.attack);
          json.at("use_time").get_to(feature.use_time);
          data.feature = feature;
        }
        break;
    }

  }

}
