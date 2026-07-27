#ifndef FW_WORLD_RUNTIME_H
#define FW_WORLD_RUNTIME_H

#include <vector>

#include <gf2/core/Random.h>

#include "Date.h"
#include "HeroRuntime.h"
#include "MapRuntime.h"
#include "NetworkRuntime.h"
#include "WorldGenerationStep.h"

namespace fw {
  struct ActorState;
  struct TrainState;
  struct WorldData;
  struct WorldState;

  struct WorldRuntime {
    Phase phase = Phase::Noon;
    gf::Vec2I view_center;
    std::optional<gf::Vec2I> mouse;
    HeroRuntime hero;
    MapRuntime map;
    NetworkRuntime network;

    gf::RectI compute_view() const;

    void set_reverse_train(const TrainState& train, uint32_t train_index);

    void bind(const WorldData& data, const WorldState& state, gf::Random* random, WorldGenerationAnalysis& analysis);

    void bind_network(const WorldState& state);
    void bind_train(const WorldState& state);
  };

}

#endif // FW_WORLD_RUNTIME_H
