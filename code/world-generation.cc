#include <atomic>

#include <gf2/core/Log.h>
#include <gf2/core/Random.h>

#include "bits/WorldGeneration.h"
#include "bits/WorldGenerationStep.h"

int main() {
  gf::Random random;
  std::atomic<fw::WorldGenerationStep> step(fw::WorldGenerationStep::Start);
  fw::generate_world(&random, step);
}
