#ifndef FW_THING_STATE_H
#define FW_THING_STATE_H

#include <gf2/core/Orientation.h>
#include <gf2/core/TypeTraits.h>

#include "Action.h"
#include "DataReference.h"
#include "Location.h"
#include "ThingData.h"

namespace fw {

  struct ThingState {
    DataReference<ThingData> data;
    Location location;
    gf::Orientation orientation;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<ThingState, Archive>& state)
  {
    return ar | state.data | state.location | state.orientation;
  }

  std::array<Action, 2> actions_from_thing(const ThingState& thing);

}

#endif // FW_THING_STATE_H
