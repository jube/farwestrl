#include "WorldRuntime.h"

#include <cassert>

#include "MapRuntime.h"
#include "NetworkState.h"
#include "Settings.h"
#include "WorldState.h"

namespace fw {

  gf::RectI WorldRuntime::compute_view() const
  {
    gf::RectI view = gf::RectI::from_center_size(view_center, GameBoxSize);
    view.offset = gf::clamp(view.offset, { 0, 0 }, WorldSize - GameBoxSize);
    return view;
  }

  void WorldRuntime::set_reverse_train(const TrainState& train, uint32_t train_index)
  {
    uint32_t offset = 0;

    for (std::size_t k = 0; k < TrainLength; ++k) {
      const uint32_t railway_index = network.next_position(train.railway_index, offset);
      assert(railway_index < network.railway.size());
      const gf::Vec2I position = network.railway[railway_index];

      for (int32_t i = -1; i <= 1; ++i) {
        for (int32_t j = -1; j <= 1; ++j) {
          const gf::Vec2I neighbor = { i, j };
          const gf::Vec2I neighbor_position = position + neighbor;
          assert(map.ground.reverse.valid(neighbor_position));
          ReverseMapCell& cell = map.ground.reverse(neighbor_position);
          assert(cell.train_index == NoIndex || train_index == NoIndex || cell.train_index == train_index);
          cell.train_index = train_index;
        }
      }

      offset += 3;
    }
  }

  void WorldRuntime::bind([[maybe_unused]] const WorldData& data, const WorldState& state, gf::Random* random, WorldGenerationAnalysis& analysis)
  {
    view_center = state.hero().position;
    map.bind(state, random, analysis);

    analysis.set_step(WorldGenerationStep::Network);
    bind_network(state);
    bind_train(state);
  }

  void WorldRuntime::bind_network(const WorldState& state) {
    const std::vector<gf::Vec2I>& railway = state.network.railway;

    for (std::size_t i = 0; i < railway.size(); ++i) {
      const std::size_t j = (i + 1) % railway.size();

      const gf::Vec2I curr_position = railway[i];
      const gf::Vec2I next_position = railway[j];
      const gf::Vec2I step = gf::sign(next_position - curr_position);

      for (gf::Vec2I position = curr_position; position != next_position; position += step) {
        assert(network.railway.empty() || gf::manhattan_distance(network.railway.back(), position) == 1);
        network.railway.push_back(position);
      }
    }

    assert(gf::manhattan_distance(network.railway.back(), network.railway.front()) == 1);
  }

  void WorldRuntime::bind_train(const WorldState& state)
  {
    for (const auto& [ train_index, train ] : gf::enumerate(state.network.trains)) {
      set_reverse_train(train, static_cast<uint32_t>(train_index));
    }
  }

}
