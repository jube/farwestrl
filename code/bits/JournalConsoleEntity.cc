#include "JournalConsoleEntity.h"

#include <cstdint>

#include <ranges>

#include <gf2/core/ConsoleOperations.h>

#include "FarWest.h"
#include "JournalState.h"
#include "Settings.h"
#include "WorldState.h"

namespace fw {

  JournalConsoleEntity::JournalConsoleEntity(FarWest* game)
  : m_game(game)
  {
  }

  void JournalConsoleEntity::update([[maybe_unused]] gf::Time time)
  {
  }

  void JournalConsoleEntity::render(gf::Console& console)
  {
    const WorldState* state = m_game->state();
    const std::vector<JournalEntryState>& entries = state->journal.entries;
    int32_t count = 0;
    std::string journal;

    for (const JournalEntryState& entry : entries | std::views::reverse) {
      journal += fmt::format("<style=date>{}</>: {}\n", entry.date.to_string_hours_minutes(), entry.message);
      ++count;

      if (count == MessageBoxSize.h) {
        break;
      }
    }

    gf::console_print_text(console, MessageBox, gf::ConsoleAlignment::Left, m_game->style(), "{}", journal);
  }

}
