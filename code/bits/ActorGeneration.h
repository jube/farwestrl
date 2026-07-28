#ifndef FW_ACTOR_GENERATION_H
#define FW_ACTOR_GENERATION_H

#include <gf2/core/Random.h>

#include "ActorState.h"
#include "Location.h"

namespace fw {
  struct WorldData;

  ActorState generate_animal(std::string_view tag, Location location, const WorldData& world_data, gf::Random* random);
  ActorState generate_human(std::string_view tag, Location location, const WorldData& world_data, gf::Random* random);

}

#endif // FW_ACTOR_GENERATION_H
