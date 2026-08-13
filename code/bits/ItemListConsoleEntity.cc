#include "ItemListConsoleEntity.h"

#include <gf2/core/ConsoleOperations.h>

#include "Colors.h"
#include "ItemState.h"
#include "FarWest.h"

namespace fw {

  namespace {

    constexpr int32_t InventoryListLength = 22;

  }

  ItemListConsoleEntity::ItemListConsoleEntity(FarWest* game)
  : m_game(game)
  {
  }

  void ItemListConsoleEntity::set_inventory(const InventoryState* state)
  {
    m_state = state;
  }

  const ItemData* ItemListConsoleEntity::current_item() const
  {
    if (m_state == nullptr) {
      return nullptr;
    }

    return m_state->items.front().data.origin;
  }

  void ItemListConsoleEntity::update([[maybe_unused]] gf::Time time)
  {

  }

  void ItemListConsoleEntity::render(gf::Console& console)
  {
    assert(console.size() == ItemListConsoleSize);

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

    gf::console_clear(console, style);
    gf::console_draw_frame(console, gf::RectI::from_size(ItemListConsoleSize), style);

    if (!m_state->items.empty()) {
      gf::Vec2I position = { 2, 2 };

      for (const InventoryItemState& item : m_state->items) {
        gf::console_print_text(console, position, gf::ConsoleAlignment::Left, rich_style, "{:.<20}", item.data->label.tag);
        gf::console_print_text(console, position + gf::dirx(InventoryListLength), gf::ConsoleAlignment::Right, rich_style, "{:.>3}", item.count);
        ++position.y;
      }

    }
  }


}
