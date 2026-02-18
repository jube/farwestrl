#ifndef FW_ACTION_H
#define FW_ACTION_H

#include <cstdint>

#include <gf2/core/TaggedVariant.h>
#include <gf2/core/Vec2.h>

namespace fw {
  struct WorldModel;
  struct ActorState;

  enum class ActionType {
    None,
    Idle,
    Move,
    Mount,
    Dismount,
    Reload,
    Graze,
  };

  struct IdleAction {
    uint16_t time;
  };

  struct MoveAction {
    gf::Vec2I displacement = { 0, 0 };
  };

  struct MountAction {
  };

  struct DismountAction {
  };

  struct ReloadAction {
  };

  struct GrazeAction {
    gf::Vec2I displacement = { 0, 0 };
  };


  template<typename T, typename ... Args>
  T make_action(Args&& ... args)
  {
    return { std::forward<Args>(args)... };
  }


  using Action = gf::TaggedVariant<ActionType, IdleAction, MoveAction, MountAction, DismountAction, ReloadAction, GrazeAction>;

  enum class ActionResult : uint8_t {
    Failure,
    Success,
  };

  ActionResult compute_action(WorldModel& model, ActorState& actor, const Action& action);

}

#endif // FW_ACTION_H
