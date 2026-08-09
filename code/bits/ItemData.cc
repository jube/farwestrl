#include "ItemData.h"

#include <fstream>
#include <ranges>
#include <string>

#include <gf2/core/Log.h>
#include <gf2/core/StringUtils.h>

#include "config.h"

namespace fw {

  namespace {

    gf::Console load_image(const std::filesystem::path& path, gf::Color color)
    {
      gf::Console console({ 20, 20 });

      std::filesystem::path directory = FarWestDataDirectory;
      std::ifstream input(directory / "images" / path);

      if (!input) {
        gf::Log::fatal("No image file: {}", path.string());
      }

      std::string image_string;

      for (std::string line; std::getline(input, line); ) {
        image_string.append(std::move(line));
      }

      for (auto [ cell, character ] : std::views::zip(console, gf::codepoints(image_string))) {
        cell.mode = gf::ConsoleMode::Picture;
        cell.parts[0].character = static_cast<char16_t>(character);
        cell.parts[0].foreground = color;
        cell.parts[0].background = gf::White;
      }

      return console;
    }

  }

  NLOHMANN_JSON_SERIALIZE_ENUM( ItemType, {
    { ItemType::None, nullptr },
    { ItemType::MeleeWeapon, "melee_weapon" },
    { ItemType::DistanceWeapon, "distance_weapon" },
    { ItemType::Projectile, "projectile" },
  })

  NLOHMANN_JSON_SERIALIZE_ENUM( ProjectileKind, {
    { ProjectileKind::None, nullptr },
    { ProjectileKind::Dot36Ammunition, ".36" },
    { ProjectileKind::Dot44Ammunition, ".44" },
  })

  void from_json(const nlohmann::json& json, ItemData& data)
  {
    json.at("label").get_to(data.label);
    json.at("display").get_to(data.display);

    std::filesystem::path image_path;
    json.at("image").get_to(image_path);
    data.image = load_image(image_path, data.display.color);

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
          json.at("empty").get_to(feature.empty);
          json.at("attack").get_to(feature.attack);
          json.at("kind").get_to(feature.kind);
          json.at("modifier").get_to(feature.modifier);
          data.feature = feature;
        }
        break;
    }

  }

}
