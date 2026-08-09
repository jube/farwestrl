#include "InventoryConsoleEntity.h"

#include <gf2/core/ConsoleOperations.h>

#include "Colors.h"
#include "ItemState.h"
#include "Settings.h"
#include "gf2/core/ConsoleStyle.h"

namespace fw {

  namespace {

    constexpr gf::Vec2I InventoryConsoleSize = { 60, 45 };
    constexpr gf::Vec2I InventoryConsolePosition = (GameBoxSize - InventoryConsoleSize) / 2;

    constexpr gf::RectI InventoryListBox = gf::RectI::from_position_size({ 1, 1 }, { 29, 43 });
    constexpr gf::RectI InventoryPreviewBox = gf::RectI::from_position_size({ 30, 1 }, { 29, 43 });

    static_assert(InventoryListBox.offset.x + InventoryListBox.extent.x == InventoryPreviewBox.offset.x);
    static_assert(InventoryPreviewBox.offset.x + InventoryPreviewBox.extent.x + 1 == InventoryConsoleSize.x);

    constexpr gf::Vec2I InventoryImagePosition = { InventoryPreviewBox.offset.x + (InventoryPreviewBox.extent.x - 20) / 2, 0 + 2 };

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

    gf::console_clear(m_console, style);
    gf::console_draw_frame(m_console, gf::RectI::from_size(InventoryConsoleSize), style);

    gf::console_draw_frame(m_console, InventoryListBox, style);
    gf::console_draw_frame(m_console, InventoryPreviewBox, style);


    if (!m_state->items.empty()) {
      const InventoryItemState& item = m_state->items.front();
      gf::console_blit_to(item.data->image, m_console, InventoryImagePosition);

      gf::console_print_text(m_console, { 44, 23 }, gf::ConsoleAlignment::Center, style, "{}", item.data->label.tag);

    }


    gf::console_blit_to(m_console, console, InventoryConsolePosition);
  }


}
