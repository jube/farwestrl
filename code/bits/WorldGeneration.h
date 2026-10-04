#ifndef FW_WORLD_GENERATION_H
#define FW_WORLD_GENERATION_H

#include <gf2/core/Random.h>

#include "Analysis.h"
#include "WorldState.h"

namespace fw {

  WorldState generate_world(gf::Random* random, const WorldData& data, Analysis& analysis);

}

#endif // FW_WORLD_GENERATION_H
