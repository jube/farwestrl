#ifndef FW_BEHAVIOR_H
#define FW_BEHAVIOR_H

#include <map>
#include <optional>

#include <gf2/core/BehaviorTree.h>
#include <gf2/core/Random.h>

#include "Action.h"

namespace fw {
  struct WorldModel;
  struct ActorState;

  struct BehaviorBlackboard {
    const WorldModel* model = nullptr;
    ActorState* actor = nullptr;
    gf::Random* random = nullptr;
    std::optional<Action> action;
  };

  class BehaviorManager {
  public:
    BehaviorManager();

    Action select_behavior(const WorldModel& model, ActorState& actor, gf::Random* random);

  private:
    std::map<gf::Id, gf::behavior::AnyBehavior<BehaviorBlackboard>> m_trees;
  };

}

#endif // FW_BEHAVIOR_H
