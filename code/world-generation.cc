#include <gf2/core/Log.h>
#include <gf2/core/Random.h>

#include "bits/WorldGeneration.h"
#include "bits/WorldGenerationStep.h"

int main() {
  gf::Random random;
  fw::WorldGenerationAnalysis analysis;
  fw::generate_world(&random, analysis);
  analysis.print_analysis();
}
