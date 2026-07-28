#ifndef FW_SCHEDULER_STATE_H
#define FW_SCHEDULER_STATE_H

#include <gf2/core/BinaryHeap.h>
#include <gf2/core/TaggedVariant.h>
#include <gf2/core/TypeTraits.h>
#include <gf2/core/Vec2.h>

#include "Date.h"
#include "Index.h"

namespace fw {

  struct Task {
    Date date;
    uint32_t index;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<Task, Archive>& task)
  {
    return ar | task.date | task.index;
  }

  bool operator<(const Task& lhs, const Task& rhs);

  struct SchedulerState {
    gf::BinaryHeap<Task> queue;

    bool is_hero_turn() const
    {
      const Task& top = queue.top();
      return top.index == HeroIndex;
    }

  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<SchedulerState, Archive>& state)
  {
    return ar | state.queue;
  }

}

#endif // FW_SCHEDULER_STATE_H
