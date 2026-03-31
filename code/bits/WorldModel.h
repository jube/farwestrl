#ifndef FW_WORLD_MODEL_H
#define FW_WORLD_MODEL_H

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
    bool is_running() const { return m_phase == ModelPhase::Running; }

    uint32_t index_of(ActorState& actor) const;

    bool is_walkable(Floor floor, gf::Vec2I position) const;

    void update_current_task_in_queue(uint16_t seconds);


    bool check() const;

  private:
    gf::Random* m_random = nullptr;

    enum class ModelPhase {
      Running,
      Cooldown,
    };

    ModelPhase m_phase = ModelPhase::Running;
    gf::Time m_cooldown;

    void update_date();
    bool update_hero();
    bool update_train(TrainState& train, uint32_t train_index);

  };

}

#endif // FW_WORLD_MODEL_H
