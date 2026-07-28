#ifndef FW_BODY_STATE_H
#define FW_BODY_STATE_H

#include <cstdint>

#include <gf2/core/Fixed.h>
#include <gf2/core/TypeTraits.h>

namespace fw {

  using Stat = gf::Fixed<int32_t, 16>;

  struct BodyState {
    int8_t health;
    // attributes
    int8_t force;
    int8_t dexterity;
    int8_t constitution;
    int8_t luck; // hidden
    // stats
    Stat intensity;
    Stat precision;
    Stat endurance;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<BodyState, Archive>& state)
  {
    return ar | state.health | state.force | state.dexterity | state.constitution | state.luck | state.intensity | state.precision | state.endurance;
  }

}

#endif // FW_BODY_STATE_H
