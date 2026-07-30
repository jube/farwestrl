#include "ItemData.h"

namespace fw {

  NLOHMANN_JSON_SERIALIZE_ENUM( ItemType, {
    { ItemType::None, nullptr },
    { ItemType::MeleeWeapon, "melee_weapon" },
    { ItemType::DistanceWeapon, "distance_weapon" },
    { ItemType::Projectile, "projectile" },
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
      case ItemType::MeleeWeapon:
        {
          MeleeWeaponDataFeature feature;
          json.at("attack").get_to(feature.attack);
          json.at("use_time").get_to(feature.use_time);
          json.at("modifier").get_to(feature.modifier);
          data.feature = feature;
        }
        break;
      case ItemType::DistanceWeapon:
        {
          DistanceWeaponDataFeature feature;
          json.at("range").get_to(feature.range);
          json.at("reload_time").get_to(feature.reload_time);
          json.at("shoot_time").get_to(feature.shoot_time);
          json.at("capacity").get_to(feature.capacity);
          json.at("modifier").get_to(feature.modifier);
          json.at("projectile").get_to(feature.projectile);
          data.feature = feature;
        }
        break;
      case ItemType::Projectile:
        {
          ProjectileDataFeature feature;
          json.at("attack").get_to(feature.attack);
          json.at("modifier").get_to(feature.modifier);
          data.feature = feature;
        }
        break;
    }

  }

}
