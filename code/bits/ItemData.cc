#include "ItemData.h"

#include <fstream>
#include <ranges>
#include <string>

#include <gf2/core/Log.h>
#include <gf2/core/StringUtils.h>

#include "Colors.h"
#include "Settings.h"

#include "config.h"

namespace fw {

  namespace {

    constexpr gf::Color PaperColor = 0xE0C9A6; // #E0C9A6

    gf::Console load_image(const std::filesystem::path& path)
    {
      std::filesystem::path directory = FarWestDataDirectory;
      std::ifstream input(directory / "images" / path);

      if (!input) {
        gf::Log::fatal("No image file: {}", path.string());
      }

      // load the image into a string

      std::string image_string;

      for (std::string line; std::getline(input, line); ) {
        image_string.append(std::move(line));
      }

      // create a random generator based on the hash of the string

      gf::Random random(static_cast<uint64_t>(gf::hash_string(image_string)));

      // put the string in the console

      gf::Console console({ ItemImageSize, ItemImageSize });

      for (auto [ cell, character ] : std::views::zip(console, gf::codepoints(image_string))) {
        cell.mode = gf::ConsoleMode::Picture;
        gf::ConsoleCellPart& part = cell.parts[0];

        part.character = static_cast<char16_t>(character);
        part.foreground = gf::Black;
        part.background = gf::darker(PaperColor, random.compute_uniform_float(0.04f));

        // put a hole in the paper sometimes

        if (character == U' ' && random.compute_bernoulli(0.01)) {
          part.character = u'\u25D8'; /* '◘' */
          part.foreground = part.background;
          part.background = RpgBlue;
        }
      }

      // modify the outline of the paper

      auto fill_corner = [&](gf::Vec2I position, char16_t picture) {
        assert(console.valid(position));
        gf::ConsoleCellPart& part = console(position).parts[0];

        if (random.compute_bernoulli(0.5)) {
          part.character = picture;
        } else {
          part.character = u'\u2588'; /* '█' */
        }

        part.foreground = part.background;
        part.background = RpgBlue;
      };

      fill_corner({ 0,                 0                 }, u'\u25E2' /* '◢' */);
      fill_corner({ ItemImageSize - 1, 0                 }, u'\u25E3' /* '◣' */);
      fill_corner({ 0,                 ItemImageSize - 1 }, u'\u25E5' /* '◥' */);
      fill_corner({ ItemImageSize - 1, ItemImageSize - 1 }, u'\u25E4' /* '◤' */);

      auto fill_outline = [&](gf::Vec2I start, gf::Vec2I direction, char16_t hi_picture, char16_t lo_picture) {
        bool is_in_high_spot = true;
        bool has_just_changed = true;

        for (int32_t i = 0; i < ItemImageSize - 2; ++i) {
          const gf::Vec2I position = start + i * direction;
          assert(console.valid(position));
          gf::ConsoleCellPart& part = console(position).parts[0];

          if (part.character != u' ') {
            assert(is_in_high_spot);
            has_just_changed = false;
          } else {
            const gf::Vec2I next_position = position + direction;
            assert(console.valid(position));

            if (console(next_position).parts[0].character != u' ') {
              if (!is_in_high_spot) {
                part.character = hi_picture;
                part.foreground = part.background;
                part.background = RpgBlue;
                is_in_high_spot = true;
                has_just_changed = true;
              }
            } else if (!has_just_changed && random.compute_bernoulli(0.4)) {
              part.character = is_in_high_spot ? lo_picture : hi_picture;
              part.foreground = part.background;
              part.background = RpgBlue;
              is_in_high_spot = !is_in_high_spot;
              has_just_changed = true;
            } else if (!is_in_high_spot) {
              part.background = RpgBlue;
              has_just_changed = false;
            } else {
              has_just_changed = false;
            }
          }
        }
      };

      fill_outline({ 0,                 1 }, { 0, 1 }, u'\u25E2' /* '◢' */, u'\u25E5' /* '◥' */);
      fill_outline({ ItemImageSize - 1, 1 }, { 0, 1 }, u'\u25E3' /* '◣' */, u'\u25E4' /* '◤' */);
      fill_outline({ 1, 0                 }, { 1, 0 }, u'\u25E2' /* '◢' */, u'\u25E3' /* '◣' */);
      fill_outline({ 1, ItemImageSize - 1 }, { 1, 0 }, u'\u25E5' /* '◥' */, u'\u25E4' /* '◤' */);

      // it's done!

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

  std::string_view to_string(ItemType type)
  {
    switch (type) {
      case ItemType::None:
        return "None";
      case ItemType::MeleeWeapon:
        return "Melee Weapon";
      case ItemType::DistanceWeapon:
        return "Distance Weapon";
      case ItemType::Projectile:
        return "Projectile";
    }

    return "<unknown>";
  }

  std::string_view to_string(ProjectileKind kind)
  {
    switch (kind) {
      case ProjectileKind::None:
        return "None";
      case ProjectileKind::Dot36Ammunition:
        return ".36";
      case ProjectileKind::Dot44Ammunition:
        return ".44";
    }

    return "<unknown>";
  }

  void from_json(const nlohmann::json& json, ItemData& data)
  {
    json.at("label").get_to(data.label);
    json.at("display").get_to(data.display);

    std::filesystem::path image_path;
    json.at("image").get_to(image_path);
    data.image = load_image(image_path);

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
