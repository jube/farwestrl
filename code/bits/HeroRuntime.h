#ifndef FW_HERO_RUNTIME_H
#define FW_HERO_RUNTIME_H

#include <vector>

#include <gf2/core/TaggedVariant.h>
#include <gf2/core/Vec2.h>

#include "Action.h"

namespace fw {

  struct HeroRuntime {
    Action action;
    std::vector<gf::Vec2I> moves;

    void idle()
    {
      action = make_action<IdleAction>();
    }

    void move(gf::Vec2I orientation)
    {
      action = make_action<MoveAction>(orientation);
    }

    void mount()
    {
      action = make_action<MountAction>();
    }

    void dismount()
    {
      action = make_action<DismountAction>();
    }

    void reload()
    {
      action = make_action<ReloadAction>();
    }

  };

}

#endif // FW_HERO_RUNTIME_H
