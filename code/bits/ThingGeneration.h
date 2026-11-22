#ifndef FW_THING_GENERATION_H
#define FW_THING_GENERATION_H

#include <gf2/core/Random.h>

#include "ThingState.h"

namespace fw {
  struct WorldData;

  ThingState generate_thing(ThingType type, ThingQuality quality, Location location, gf::Direction direction, const WorldData& data, gf::Random* random);

}

#endif // FW_THING_GENERATION_H
