#ifndef FW_ACTION_H
#define FW_ACTION_H

#include <cstdint>

#include <gf2/core/TaggedVariant.h>
#include <gf2/core/Vec2.h>

#include "DateTypes.h"
#include "Location.h"

namespace fw {
  struct WorldModel;
  struct ActorState;

  enum class ActionType : uint16_t {
    None,
    Idle,
    Move,
    Mount,
    Dismount,
    Reload,
    Graze,
    Wander,
    Cruise,
    LockDoor,
    UnlockDoor,
    OpenDoor,
    CloseDoor,
  };

  struct IdleAction {
    Second time;
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

  struct WanderAction {
    gf::Vec2I displacement = { 0, 0 };
  };

  struct CruiseAction {
  };

  struct LockDoorAction {
    Location door;
  };

  struct UnlockDoorAction {
    Location door;
  };

  struct OpenDoorAction {
    Location door;
  };

  struct CloseDoorAction {
    Location door;
  };

  template<typename T, typename ... Args>
  T make_action(Args&& ... args)
  {
    return { std::forward<Args>(args)... };
  }


  using Action = gf::TaggedVariant<ActionType,
    IdleAction,
    MoveAction,
    MountAction,
    DismountAction,
    ReloadAction,
    GrazeAction,
    WanderAction,
    CruiseAction,
    LockDoorAction,
    UnlockDoorAction,
    OpenDoorAction,
    CloseDoorAction
  >;

  enum class ActionResult : uint8_t {
    Failure,
    Success,
  };

  ActionResult compute_action(WorldModel& model, ActorState& actor, const Action& action);

}

#endif // FW_ACTION_H
