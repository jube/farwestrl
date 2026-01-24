#ifndef FW_WORLD_GENERATION_H
#define FW_WORLD_GENERATION_H

#include <gf2/core/Random.h>

#include "WorldState.h"
#include "WorldGenerationStep.h"

namespace fw {

  WorldState generate_world(gf::Random* random, WorldGenerationAnalysis& analysis);

}

#endif // FW_WORLD_GENERATION_H
