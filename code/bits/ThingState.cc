#include "ThingState.h"

namespace fw {

  std::array<Action, 2> actions_from_thing(const ThingState& thing)
  {
    assert(thing.data);

    switch (thing.data->type) {
      case ThingType::None:
        return { };
      case ThingType::Bed:
        // TODO: rest and sleep
        break;
      case ThingType::SaloonChair:
        // TODO: drink
        break;
      case ThingType::SaloonDesk:
        // TODO: drink and talk
        break;
    }

    return { };
  }

}
