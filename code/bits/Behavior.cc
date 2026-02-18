#include "Behavior.h"

#include <cassert>

#include "Action.h"
#include "ActorState.h"
#include "Times.h"
#include "WorldModel.h"

namespace fw {

  namespace {
    /*
     * Helpers
     */

    gf::Orientation random_orientation(gf::Random* random) {
      constexpr gf::Orientation Orientations[] = {
        gf::Orientation::Center,
        gf::Orientation::North,
        gf::Orientation::NorthEast,
        gf::Orientation::East,
        gf::Orientation::SouthEast,
        gf::Orientation::South,
        gf::Orientation::SouthWest,
        gf::Orientation::West,
        gf::Orientation::NorthWest,
      };

      const std::size_t index = random->compute_uniform_integer(std::size(Orientations));
      assert(index < std::size(Orientations));
      return Orientations[index];
    }

    /*
     * Behaviors
     */

    Action select_cow_behavior([[maybe_unused]] const WorldModel& model, ActorState& actor, gf::Random* random)
    {
      assert(actor.feature.type() == ActorType::Animal);

      if (actor.feature.from<ActorType::Animal>().mounted_by != NoIndex) {
        return make_action<IdleAction>(IdleTime);
      }

      // TODO: change this when group behavior is added

      const gf::Orientation orientation = random_orientation(random);
      return make_action<GrazeAction>(gf::displacement(orientation));
    }

  }


  Action select_behavior(const WorldModel& model, ActorState& actor, gf::Random* random)
  {
    using namespace gf::literals;

    if (model.index_of(actor) == 0) {
      return model.runtime.hero.action;
    }

    switch (actor.data->label.id) {
      case "Cow"_id:
        return select_cow_behavior(model, actor, random);

      default:
        assert(false);
        break;
    }

    return make_action<IdleAction>(IdleTime);
  }

}
