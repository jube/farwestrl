#include "HeroConsoleEntity.h"

#include <cassert>

#include <gf2/core/ConsoleOperations.h>

#include "Colors.h"
#include "FarWest.h"
#include "ItemData.h"
#include "Settings.h"

// #include "gf2/core/Color.h"
// #include "gf2/core/ConsoleEffect.h"
// #include "gf2/core/ConsoleStyle.h"

namespace fw {

  namespace {

    std::string_view gender_style(Gender gender)
    {
      switch (gender) {
        case Gender::Girl:
          return "girl";
        case Gender::Boy:
          return "boy";
        case Gender::NonBinary:
          return "non_binary";
      }

      assert(false);
      return "unknown";
    }

    std::string_view gender_symbol(Gender gender)
    {
      switch (gender) {
        case Gender::Girl:
          return "♀";
        case Gender::Boy:
          return "♂";
        case Gender::NonBinary:
          return "○";
      }

      assert(false);
      return "#";
    }

    std::string health_bar(int8_t health)
    {
      std::string health_string;

      for (int8_t i = 0; i < health; ++i) {
        health_string += "♥";
      }

      return health_string;
    }

  }

  HeroConsoleEntity::HeroConsoleEntity(FarWest* game)
  : m_game(game)
  {
  }

  void HeroConsoleEntity::update([[maybe_unused]] gf::Time time)
  {
  }

  void HeroConsoleEntity::render(gf::Console& console)
  {
    WorldState *state = m_game->state();

    gf::ConsoleStyle character_box_style;
    character_box_style.color.foreground = gf::Gray;
    gf::console_draw_frame(console, CharacterBox, character_box_style);

    gf::Vec2I position = CharacterBoxPosition + 1;

    gf::console_print_text(console, position, gf::ConsoleAlignment::Left, m_game->style(), "<style=date>{}</>", state->current_date.to_string());
    position.y += 2;

    const ActorState& hero = state->hero();
    const HumanFeature& feature = hero.feature.from<ActorType::Human>();

    gf::console_print_text(console, position, gf::ConsoleAlignment::Left, m_game->style(), "<style=hero>{}</>", feature.name);
    ++position.y;
    gf::console_print_picture(console, position, gf::ConsoleAlignment::Left, m_game->style(), "<style={}>{}</>", gender_style(feature.gender), gender_symbol(feature.gender));
    gf::console_print_text(console, position + gf::dirx(1), gf::ConsoleAlignment::Left, m_game->style(), "{} year old", feature.age);

    position.y += 2;

    gf::console_print_picture(console, position + gf::dirx(1), gf::ConsoleAlignment::Left, m_game->style(), "<style=health>{}</><style=non_health>{}</>", health_bar(feature.health), health_bar(MaxHealth - feature.health));

    position.y += 2;

    gf::ConsoleStyle stat_style;
    stat_style.effect = gf::ConsoleEffect::add();
    stat_style.color.foreground = gf::Black;
    stat_style.color.background = gf::Transparent;

    auto print_attribute_stat = [&](std::string_view attribute_name, std::string_view attribute_style, int8_t attribute, std::string_view stat_name, gf::Color stat_color, const Stat& stat) {
      gf::console_print_text(console, position, gf::ConsoleAlignment::Left, m_game->style(), "<style={}>{}</>: {}", attribute_style, attribute_name, attribute);
      ++position.y;

      const int stat_bar = (stat * (CharacterBoxSize.w - 2) / 100).as_int();

      for (int x = 0; x < CharacterBoxSize.w - 2; ++x) {
        gf::console_write_background(console, position + gf::dirx(x), x <= stat_bar ? stat_color : gf::Gray);
      }

      gf::console_print_text(console, position, gf::ConsoleAlignment::Left, stat_style, "{}: {}", stat_name, stat.as_int());
      ++position.y;
    };

    print_attribute_stat("FOR", "force", feature.force, "Intensity", ForceColor, feature.intensity);
    print_attribute_stat("DEX", "dexterity", feature.dexterity, "Precision", DexterityColor, feature.precision);
    print_attribute_stat("CON", "constitution", feature.constitution, "Endurance", ConstitutionColor, feature.endurance);

    ++position.y;

    gf::console_print_text(console, position, gf::ConsoleAlignment::Left, m_game->style(), "<style=weapon>Weapon</>: {}", hero.weapon.data ? hero.weapon.data->label.tag : "-");
    ++position.y;

    if (hero.ammunition.data) {
      assert(hero.ammunition.data->feature.type() == ItemType::Ammunition);
      const AmmunitionDataFeature& ammunition = hero.ammunition.data->feature.from<ItemType::Ammunition>();
      gf::console_print_text(console, position, gf::ConsoleAlignment::Left, m_game->style(), "<style=weapon>Ammunitions</>: .{} × {}", ammunition.caliber, hero.ammunition.count); // only for firearms

      assert(hero.weapon.data->feature.type() == ItemType::Firearm);
      const FirearmDataFeature& firearm = hero.weapon.data->feature.from<ItemType::Firearm>();

      std::string cartridges;

      for (int8_t i = 0; i < hero.weapon.cartridges; ++i) {
        cartridges += "•";
      }

      for (int8_t i = hero.weapon.cartridges; i < firearm.capacity; ++i) {
        cartridges += "○";
      }

      gf::console_print_picture(console, position + gf::dirx(CharacterBoxSize.x - 3), gf::ConsoleAlignment::Right, m_game->style(), "{}", cartridges);
    } else {
      gf::console_print_text(console, position, gf::ConsoleAlignment::Left, m_game->style(), "<style=weapon>Ammunitions</>: -");
      gf::console_print_picture(console, position + gf::dirx(CharacterBoxSize.x - 3), gf::ConsoleAlignment::Right, m_game->style(), "∅");
    }

    position.y += 2;

    gf::console_print_text(console, position, gf::ConsoleAlignment::Left, m_game->style(), "<style=cash>Cash</>: 100$");
    ++position.y;
    gf::console_print_text(console, position, gf::ConsoleAlignment::Left, m_game->style(), "<style=debt>Debt</>: 10034$");
    ++position.y;


  }

}
