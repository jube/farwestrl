#include "ItemPreviewConsoleEntity.h"

#include <gf2/core/ConsoleOperations.h>

#include "FarWest.h"
#include "ItemData.h"
#include "Settings.h"
#include "Styles.h"

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

    const gf::ConsoleStyle& style = ui_default_style();
    const gf::ConsoleRichStyle& rich_style = ui_rich_style();

    gf::console_clear(console, style);
    gf::console_draw_frame(console, gf::RectI::from_size(ItemPreviewConsoleSize), style);

    gf::console_blit_to(m_data->image, console, ItemPreviewImagePosition);

    gf::console_print_text(console, ItemPreviewNamePosition, gf::ConsoleAlignment::Center, rich_style, "<style=item>{}</>", m_data->label.tag);

    gf::Vec2I position = ItemPreviewDescriptionPosition;

    auto print_key_value = [&]<typename... T>(std::string_view property, fmt::format_string<T...> fmt, T&&... value) {
      gf::console_print_text(console, position, gf::ConsoleAlignment::Left, rich_style, "<style=property>{}</>:", property);
      gf::console_print_text(console, position + gf::dirx(ItemPreviewDescriptionWidth), gf::ConsoleAlignment::Right, rich_style, fmt, std::forward<T>(value)...);
      ++position.y;
    };

    print_key_value("Type", "{}", to_string(m_data->type()));

    gf::console_print_text(console, position, gf::ConsoleAlignment::Left, rich_style, "<style=property>Display</>:");
    gf::console_write_picture(console, position + gf::dirx(ItemPreviewDescriptionWidth), m_data->display.picture, { m_data->display.color, gf::White });
    ++position.y;

    ++position.y;

    switch (m_data->type()) {
      case ItemType::None:
        break;
      case ItemType::Container:
      {
        const ContainerElement& element = m_data->element.from<ItemType::Container>();
        print_key_value("Capacity", "{}", element.capacity);
        break;
      }
      case ItemType::MeleeWeapon:
      {
        const MeleeWeaponElement& element = m_data->element.from<ItemType::MeleeWeapon>();
        print_key_value("Attack", "{}", element.attack.as_int());
        print_key_value("Modifier", "{:+d}", element.modifier);
        print_key_value("Use Time", "{}s", element.use_time);
        break;
      }
      case ItemType::DistanceWeapon:
      {
        const DistanceWeaponElement& element = m_data->element.from<ItemType::DistanceWeapon>();
        print_key_value("Projectile", "{}", to_string(element.projectile));
        print_key_value("Capacity", "{}", element.capacity);
        print_key_value("Range", "{}m", element.range);
        print_key_value("Modifier", "{:+d}", element.modifier);
        print_key_value("Shoot Time", "{}s", element.shoot_time);
        print_key_value("Reload Time", "{}s", element.reload_time);
        break;
      }
      case ItemType::Projectile:
      {
        const ProjectileElement& element = m_data->element.from<ItemType::Projectile>();
        print_key_value("Kind", "{}", to_string(element.kind));
        print_key_value("Attack", "{}", element.attack.as_int());
        print_key_value("Modifier", "{:+d}", element.modifier);
        break;
      }
    }
  }


}
