#include <gf2/core/Clock.h>
#include <gf2/core/Log.h>
#include <gf2/core/Random.h>

#include "bits/Times.h"
#include "bits/WorldGeneration.h"
#include "bits/WorldGenerationStep.h"
#include "bits/WorldModel.h"

#include "config.h"

int main() {
  const std::filesystem::path data_directory = fw::FarWestDataDirectory;

  gf::Random random;
  fw::WorldGenerationAnalysis analysis;

  fw::WorldModel model(&random);

  analysis.set_step(fw::WorldGenerationStep::File);
  model.data.load_from_file(data_directory / "data.json");
  model.state = fw::generate_world(&random, analysis);
  model.bind(analysis);
  analysis.print_analysis();

  gf::Clock clock;

  for (;;) {
    if (model.is_running()) {
      gf::Log::debug("Step: {} µs", clock.restart().as_microseconds());
    } else {
      gf::Log::debug("Cooldown");
    }

    model.runtime.hero.action = fw::make_action<fw::IdleAction>(fw::IdleTime);
    model.update(gf::seconds(1.0f / 60.0f));
  }

}
