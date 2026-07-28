#include "ActorState.h"

namespace fw {

  Location ActorState::location() const
  {
    if (feature.type() == ActorType::Human) {
      return feature.from<ActorType::Human>().location;
    }

    if (feature.type() == ActorType::Animal) {
      return feature.from<ActorType::Animal>().location;
    }

    return {};
  }


}
