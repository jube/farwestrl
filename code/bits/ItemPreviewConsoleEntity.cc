#include "ItemPreviewConsoleEntity.h"

#include <gf2/core/ConsoleOperations.h>

#include "Colors.h"
#include "Settings.h"
#include "FarWest.h"

namespace fw {

  namespace {

    constexpr gf::Vec2I ItemPreviewImagePosition = { (ItemPreviewConsoleSize.x - ItemImageSize) / 2, 5 };
    constexpr gf::Vec2I ItemPreviewNamePosition = { ItemPreviewConsoleSize.x / 2, 27 };
    constexpr gf::Vec2I ItemPreviewDescriptionPosition = { ItemPreviewImagePosition.x, 30 };
    constexpr int32_t ItemPreviewDescriptionWidth = 19;

  }

  ItemPreviewConsoleEntity::ItemPreviewConsoleEntity(FarWest* game)
  : m_game(game)
  {
  }

  void ItemPreviewConsoleEntity::set_item(const ItemData* data)
  {
    m_data = data;
  }

  void ItemPreviewConsoleEntity::update([[maybe_unused]] gf::Time time)
  {

  }

  void ItemPreviewConsoleEntity::render(gf::Console& console)
  {
    assert(console.size() == ItemPreviewConsoleSize);

    if (m_data == nullptr) {
      return;
    }

    gf::ConsoleStyle style;
    style.color.foreground = gf::White;
    style.color.background = RpgBlue;
    style.effect = gf::ConsoleEffect::set();

    gf::ConsoleRichStyle rich_style;
    rich_style.set_default_style(style);
    rich_style.set_style("item", { gf::Capri, RpgBlue });
    rich_style.set_style("key", { gf::gray(0.7f), RpgBlue });

    gf::console_clear(console, style);
    gf::console_draw_frame(console, gf::RectI::from_size(ItemPreviewConsoleSize), style);

    gf::console_blit_to(m_data->image, console, ItemPreviewImagePosition);

    gf::console_print_text(console, ItemPreviewNamePosition, gf::ConsoleAlignment::Center, rich_style, "<style=item>{}</>", m_data->label.tag);

    gf::Vec2I position = ItemPreviewDescriptionPosition;

    auto print_key_value = [&]<typename... T>(std::string_view key, fmt::format_string<T...> fmt, T&&... value) {
      gf::console_print_text(console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>{}</>:", key);
      gf::console_print_text(console, position + gf::dirx(ItemPreviewDescriptionWidth), gf::ConsoleAlignment::Right, rich_style, fmt, std::forward<T>(value)...);
      ++position.y;
    };

    print_key_value("Type", "{}", to_string(m_data->type()));

    gf::console_print_text(console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Display</>:");
    gf::console_write_picture(console, position + gf::dirx(ItemPreviewDescriptionWidth), m_data->display.picture, { m_data->display.color, gf::White });
    ++position.y;

    ++position.y;

    switch (m_data->type()) {
      case ItemType::None:
        break;
      case ItemType::MeleeWeapon:
      {
        const MeleeWeaponDataFeature& feature = m_data->feature.from<ItemType::MeleeWeapon>();
        print_key_value("Attack", "{}", feature.attack.as_int());
        print_key_value("Modifier", "{:+d}", feature.modifier);
        print_key_value("Use Time", "{}s", feature.use_time);
        break;
      }
      case ItemType::DistanceWeapon:
      {
        const DistanceWeaponDataFeature& feature = m_data->feature.from<ItemType::DistanceWeapon>();
        print_key_value("Projectile", "{}", to_string(feature.projectile));
        print_key_value("Capacity", "{}", feature.capacity);
        print_key_value("Range", "{}m", feature.range);
        print_key_value("Modifier", "{:+d}", feature.modifier);
        print_key_value("Shoot Time", "{}s", feature.shoot_time);
        print_key_value("Reload Time", "{}s", feature.reload_time);
        break;
      }
      case ItemType::Projectile:
      {
        const ProjectileDataFeature& feature = m_data->feature.from<ItemType::Projectile>();
        print_key_value("Kind", "{}", to_string(feature.kind));
        print_key_value("Attack", "{}", feature.attack.as_int());
        print_key_value("Modifier", "{:+d}", feature.modifier);
        break;
      }
    }
  }


}
