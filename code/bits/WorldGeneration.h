#ifndef FW_WORLD_GENERATION_H
#define FW_WORLD_GENERATION_H

#include <atomic>

#include <gf2/core/Random.h>

#include "WorldState.h"
#include "WorldGenerationStep.h"

namespace fw {

  WorldState generate_world(gf::Random* random, std::atomic<WorldGenerationStep>& step);

}

#endif // FW_WORLD_GENERATION_H
