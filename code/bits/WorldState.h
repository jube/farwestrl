#ifndef FW_WORLD_STATE_H
#define FW_WORLD_STATE_H

#include <cassert>
#include <cstdint>

#include <filesystem>

#include <gf2/core/TypeTraits.h>

#include "ActorState.h"
#include "Date.h"
#include "DebtState.h"
#include "DoorState.h"
#include "ItemState.h"
#include "JournalState.h"
#include "MapState.h"
#include "NetworkState.h"
#include "SchedulerState.h"
#include "ThingState.h"

namespace fw {
  struct WorldData;

  constexpr std::uint16_t StateVersion = 1;

  struct WorldState {
    Date current_date;

    MapState map;
    NetworkState network;

    std::vector<ActorState> actors;
    std::vector<LocatedItemState> items;
    std::vector<ThingState> things;

    std::vector<DoorState> doors;

    DebtState debt;

    SchedulerState scheduler;
    JournalState journal;

    ActorState& hero() {
      assert(!actors.empty());
      return actors.front();
    }

    const ActorState& hero() const {
      assert(!actors.empty());
      return actors.front();
    }

    void add_message(std::string message);

    void load_from_file(const std::filesystem::path& filename);
    void save_to_file(const std::filesystem::path& filename) const;

    void bind(const WorldData& data);
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<WorldState, Archive>& state)
  {
    return ar | state.current_date | state.map | state.network | state.actors | state.things | state.doors | state.debt | state.scheduler | state.journal;
  }

}

#endif // FW_WORLD_STATE_H
