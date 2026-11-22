#include "ThingGeneration.h"

#include <ranges>

#include "ThingData.h"
#include "WorldData.h"

namespace fw {

  namespace {

    DataReference<ThingData> find_thing_data(ThingType type, ThingQuality quality, const WorldData& data, [[maybe_unused]] gf::Random* random)
    {
      std::vector<const ThingData*> exact_match = data.things
        | std::views::filter([type, quality](const ThingData& data) { return data.type == type && data.quality == quality; })
        | std::views::transform([](const ThingData& data) { return &data; })
        | std::ranges::to<std::vector<const ThingData*>>();

      assert(!exact_match.empty());

      const ThingData* chosen = exact_match.front(); // TODO: maybe at random

      const DataReference<ThingData> reference = { .tag = chosen->label.tag, .id = chosen->label.id, .origin = chosen };
      return reference;
    }

  }

  ThingState generate_thing(ThingType type, ThingQuality quality, Location location, gf::Direction direction, const WorldData& data, gf::Random* random)
  {
    ThingState state;

    state.data = find_thing_data(type, quality, data, random);
    state.location = location;
    state.direction = direction;

    return state;
  }

}
