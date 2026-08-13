#include "ItemListConsoleEntity.h"

#include <cstdint>
#include <gf2/core/ConsoleOperations.h>

#include "Colors.h"
#include "ItemState.h"
#include "FarWest.h"

namespace fw {

  namespace {

    constexpr int32_t InventoryListLength = 22;
    constexpr int32_t ItemListPageSize = ItemListConsoleSize.y - 3;

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

    [[maybe_unused]] const int32_t size = static_cast<int32_t>(m_state->items.size());
    const int32_t index = m_current_page * ItemListPageSize + m_current_index;

    assert(0 <= index && index < size);

    return m_state->items[index].data.origin;
  }

  void ItemListConsoleEntity::next_page()
  {
    ++m_current_page;
    m_current_index = 0;
    normalize_page();
  }

  void ItemListConsoleEntity::prev_page()
  {
    --m_current_page;
    m_current_index = 0;
    normalize_page();
  }

  void ItemListConsoleEntity::next_item()
  {
    ++m_current_index;
    normalize_index();
  }

  void ItemListConsoleEntity::prev_item()
  {
    --m_current_index;
    normalize_index();
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
      gf::Vec2I position = { 3, 2 };

      const int32_t size = static_cast<int32_t>(m_state->items.size());
      const int32_t start_index = m_current_page * ItemListPageSize;

      for (int32_t i = 0; i < ItemListPageSize && start_index + i < size; ++i) {
        const InventoryItemState& item = m_state->items[start_index + i];

        if (i == m_current_index) {
          gf::console_write_picture(console, position - gf::dirx(2), u'\u2192' /* '→' */, style);
        }

        gf::console_print_text(console, position, gf::ConsoleAlignment::Left, rich_style, "{:.<42}", item.data->label.tag);
        gf::console_print_text(console, position + gf::dirx(InventoryListLength), gf::ConsoleAlignment::Right, rich_style, "{}", item.count);
        ++position.y;
      }

    }
  }

  void ItemListConsoleEntity::normalize_page()
  {
    if (m_state == nullptr) {
      m_current_page = 0;
      m_current_index = 0;
    }

    const int32_t size = static_cast<int32_t>(m_state->items.size());
    const int32_t index = m_current_page * ItemListPageSize + m_current_index;

    if (index >= size) {
      const int32_t new_index = index % size;
      m_current_page = new_index / ItemListPageSize;
      m_current_index = new_index % ItemListPageSize;
    } else if (index < 0) {
      m_current_page = 0;
      m_current_index = 0;
    }
  }

  void ItemListConsoleEntity::normalize_index()
  {
    if (m_state == nullptr) {
      m_current_page = 0;
      m_current_index = 0;
    }

    if (m_current_index < 0) {
      m_current_index += ItemListPageSize;
      --m_current_page;
    } else if (m_current_index >= ItemListPageSize) {
      m_current_page += (m_current_index / ItemListPageSize);
      m_current_index = m_current_index % ItemListPageSize;
    }

    assert(0 <= m_current_index && m_current_index < ItemListPageSize);
    normalize_page();
  }

}
