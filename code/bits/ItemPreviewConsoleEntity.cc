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

    gf::console_print_text(console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Type</>:", to_string(m_data->type()));
    gf::console_print_text(console, position + gf::dirx(ItemPreviewDescriptionWidth), gf::ConsoleAlignment::Right, rich_style, "{}", to_string(m_data->type()));
    ++position.y;

    gf::console_print_text(console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Display</>:");
    gf::console_write_picture(console, position + gf::dirx(ItemPreviewDescriptionWidth), m_data->display.picture, { m_data->display.color, gf::White });
    ++position.y;

    ++position.y;

    switch (m_data->type()) {
      case ItemType::None:
        break;
      case ItemType::MeleeWeapon:
        break;
      case ItemType::DistanceWeapon:
      {
        const DistanceWeaponDataFeature& feature = m_data->feature.from<ItemType::DistanceWeapon>();
        gf::console_print_text(console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Projectile</>:");
        gf::console_print_text(console, position + gf::dirx(ItemPreviewDescriptionWidth), gf::ConsoleAlignment::Right, rich_style, "{}", to_string(feature.projectile));
        ++position.y;
        gf::console_print_text(console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Capacity</>:");
        gf::console_print_text(console, position + gf::dirx(ItemPreviewDescriptionWidth), gf::ConsoleAlignment::Right, rich_style, "{}", feature.capacity);
        ++position.y;
        gf::console_print_text(console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Range</>:");
        gf::console_print_text(console, position + gf::dirx(ItemPreviewDescriptionWidth), gf::ConsoleAlignment::Right, rich_style, "{}m", feature.range);
        ++position.y;
        gf::console_print_text(console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Modifier</>:");
        gf::console_print_text(console, position + gf::dirx(ItemPreviewDescriptionWidth), gf::ConsoleAlignment::Right, rich_style, "{:+d}", feature.modifier);
        ++position.y;
        gf::console_print_text(console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Shoot Time</>:");
        gf::console_print_text(console, position + gf::dirx(ItemPreviewDescriptionWidth), gf::ConsoleAlignment::Right, rich_style, "{}s", feature.shoot_time);
        ++position.y;
        gf::console_print_text(console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Reload Time</>:");
        gf::console_print_text(console, position + gf::dirx(ItemPreviewDescriptionWidth), gf::ConsoleAlignment::Right, rich_style, "{}s", feature.reload_time);
        ++position.y;
        break;
      }
      case ItemType::Projectile:
        break;
    }
  }


}
