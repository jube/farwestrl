#ifndef FW_LOCATION_H
#define FW_LOCATION_H

#include <gf2/core/Vec2.h>
#include <gf2/core/TypeTraits.h>

#include "MapFloor.h"

namespace fw {

  struct Location {
    gf::Vec2I position = { -1, -1 };
    Floor floor = Floor::Ground;
  };

  constexpr bool operator==(const Location& lhs, const Location& rhs)
  {
    return lhs.position == rhs.position && lhs.floor == rhs.floor;
  }

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<Location, Archive>& location)
  {
    return ar | location.position | location.floor;
  }

}

#endif // FW_LOCATION_H
