#ifndef FW_DOOR_STATE_H
#define FW_DOOR_STATE_H

#include <cstdint>

#include <gf2/core/Vec2.h>
#include <gf2/core/TypeTraits.h>

#include "Location.h"

namespace fw {

  enum class DoorStatus : uint8_t {
    Open,
    Closed,
    Locked,
  };

  enum class DoorDirection : uint8_t {
    Horizontal,
    Vertical,
  };

  struct DoorState {
    Location location;
    DoorStatus status;
    DoorDirection direction;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<DoorState, Archive>& state)
  {
    return ar | state.location | state.status | state.direction;
  }

}

#endif // DOOR_STATE_H

