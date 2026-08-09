#include "InventoryConsoleEntity.h"

#include <gf2/core/ConsoleOperations.h>

#include "Colors.h"
#include "ItemState.h"
#include "Settings.h"
#include "FarWest.h"

namespace fw {

  namespace {

    constexpr gf::Vec2I InventoryConsoleSize = { 60, 45 };
    constexpr gf::Vec2I InventoryConsolePosition = (GameBoxSize - InventoryConsoleSize) / 2;

    constexpr gf::RectI InventoryListBox = gf::RectI::from_position_size({ 1, 1 }, { 29, 43 });
    constexpr gf::RectI InventoryPreviewBox = gf::RectI::from_position_size({ 30, 1 }, { 29, 43 });

    static_assert(InventoryListBox.offset.x + InventoryListBox.extent.x == InventoryPreviewBox.offset.x);
    static_assert(InventoryPreviewBox.offset.x + InventoryPreviewBox.extent.x + 1 == InventoryConsoleSize.x);

    constexpr gf::Vec2I InventoryImagePosition = { InventoryPreviewBox.offset.x + (InventoryPreviewBox.extent.x - 20) / 2, 0 + 5 };
    constexpr gf::Vec2I InventoryNamePosition = { 44, 27 };
    constexpr gf::Vec2I InventoryBasicDescriptionPosition = { 34, 30 };
    constexpr int32_t InventoryBasicDescriptionLength = 19;

  }

  InventoryConsoleEntity::InventoryConsoleEntity(FarWest* game)
  : m_game(game)
  , m_console(InventoryConsoleSize)
  {
  }

  void InventoryConsoleEntity::set_inventory(const InventoryState* state)
  {
    m_state = state;
  }

  void InventoryConsoleEntity::update([[maybe_unused]] gf::Time time)
  {

  }

  void InventoryConsoleEntity::render(gf::Console& console)
  {
    if (m_state == nullptr) {
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

    gf::console_clear(m_console, style);
    gf::console_draw_frame(m_console, gf::RectI::from_size(InventoryConsoleSize), style);

    gf::console_draw_frame(m_console, InventoryListBox, style);
    gf::console_draw_frame(m_console, InventoryPreviewBox, style);


    if (!m_state->items.empty()) {
      const InventoryItemState& item = m_state->items.front();
      gf::console_blit_to(item.data->image, m_console, InventoryImagePosition);

      gf::console_print_text(m_console, InventoryNamePosition, gf::ConsoleAlignment::Center, rich_style, "<style=item>{}</>", item.data->label.tag);

      gf::Vec2I position = InventoryBasicDescriptionPosition;

      gf::console_print_text(m_console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Type</>:", to_string(item.data->type()));
      gf::console_print_text(m_console, position + gf::dirx(InventoryBasicDescriptionLength), gf::ConsoleAlignment::Right, rich_style, "{}", to_string(item.data->type()));
      ++position.y;

      gf::console_print_text(m_console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Display</>:");
      gf::console_write_picture(m_console, position + gf::dirx(InventoryBasicDescriptionLength), item.data->display.picture, { item.data->display.color, gf::White });
      ++position.y;

      ++position.y;

      switch (item.data->type()) {
        case ItemType::None:
          break;
        case ItemType::MeleeWeapon:
          break;
        case ItemType::DistanceWeapon:
        {
          const DistanceWeaponDataFeature& feature = item.data->feature.from<ItemType::DistanceWeapon>();
          gf::console_print_text(m_console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Projectile</>:");
          gf::console_print_text(m_console, position + gf::dirx(InventoryBasicDescriptionLength), gf::ConsoleAlignment::Right, rich_style, "{}", to_string(feature.projectile));
          ++position.y;
          gf::console_print_text(m_console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Capacity</>:");
          gf::console_print_text(m_console, position + gf::dirx(InventoryBasicDescriptionLength), gf::ConsoleAlignment::Right, rich_style, "{}", feature.capacity);
          ++position.y;
          gf::console_print_text(m_console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Range</>:");
          gf::console_print_text(m_console, position + gf::dirx(InventoryBasicDescriptionLength), gf::ConsoleAlignment::Right, rich_style, "{}m", feature.range);
          ++position.y;
          gf::console_print_text(m_console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Modifier</>:");
          gf::console_print_text(m_console, position + gf::dirx(InventoryBasicDescriptionLength), gf::ConsoleAlignment::Right, rich_style, "{:+d}", feature.modifier);
          ++position.y;
          gf::console_print_text(m_console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Shoot Time</>:");
          gf::console_print_text(m_console, position + gf::dirx(InventoryBasicDescriptionLength), gf::ConsoleAlignment::Right, rich_style, "{}s", feature.shoot_time);
          ++position.y;
          gf::console_print_text(m_console, position, gf::ConsoleAlignment::Left, rich_style, "<style=key>Reload Time</>:");
          gf::console_print_text(m_console, position + gf::dirx(InventoryBasicDescriptionLength), gf::ConsoleAlignment::Left, rich_style, "{}s", feature.reload_time);
          ++position.y;
          break;
        }
        case ItemType::Projectile:
          break;
      }
    }


    gf::console_blit_to(m_console, console, InventoryConsolePosition, 1.0f, RpgBlueAlpha);
  }


}
