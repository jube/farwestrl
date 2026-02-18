#ifndef FW_HERO_RUNTIME_H
#define FW_HERO_RUNTIME_H

#include <vector>

#include <gf2/core/Vec2.h>

#include "Action.h"

namespace fw {

  struct HeroRuntime {
    Action action;
    std::vector<gf::Vec2I> moves;
  };

}

#endif // FW_HERO_RUNTIME_H
