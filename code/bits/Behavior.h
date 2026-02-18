#ifndef FW_BEHAVIOR_H
#define FW_BEHAVIOR_H

#include <gf2/core/Random.h>

#include "Action.h"

namespace fw {
  struct WorldModel;
  struct ActorState;

  Action select_behavior(const WorldModel& model, ActorState& actor, gf::Random* random);

}

#endif // FW_BEHAVIOR_H
