#ifndef FW_WORLD_MODEL_H
#define FW_WORLD_MODEL_H

#include <atomic>

#include <gf2/core/Model.h>
#include <gf2/core/Random.h>

#include "ActorState.h"
#include "WorldData.h"
#include "WorldGenerationStep.h"
#include "WorldRuntime.h"
#include "WorldState.h"

namespace fw {

  struct WorldModel : gf::Model {
    WorldModel(gf::Random* random);

    WorldData data;
    WorldState state;
    WorldRuntime runtime;

    void bind(WorldGenerationAnalysis& analysis);

    void update(gf::Time time) override;

    uint32_t index_of(ActorState& actor) const;

    bool is_prairie(gf::Vec2I position) const;

    bool is_walkable(Floor floor, gf::Vec2I position) const;
    void move_actor(ActorState& actor, gf::Vec2I position);
    bool move_human(ActorState& actor, gf::Vec2I position);

    void update_current_task_in_queue(uint16_t seconds);

  private:
    gf::Random* m_random = nullptr;

    enum class Phase {
      Running,
      Cooldown,
    };

    Phase m_phase = Phase::Running;
    gf::Time m_cooldown;

    void update_date();

    bool update_hero();

    bool check_actor_position(ActorState& actor);
    bool change_floor(ActorState& actor, Floor new_floor);

    bool update_actor(ActorState& actor);
    void update_cow(ActorState& cow);


    bool update_train(TrainState& train, uint32_t train_index);

  };

}

#endif // FW_WORLD_MODEL_H
