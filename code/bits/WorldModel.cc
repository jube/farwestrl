#include "WorldModel.h"

#include <cassert>
#include <cstdint>

#include "Action.h"
#include "ActorData.h"
#include "ActorState.h"
#include "Behavior.h"
#include "Index.h"
#include "MapCell.h"
#include "MapRuntime.h"
#include "MapState.h"
#include "SchedulerState.h"
#include "Times.h"
#include "WorldGenerationStep.h"

namespace fw {

  namespace {

    constexpr gf::Time Cooldown = gf::milliseconds(20);

    constexpr int32_t IdleDistance = 100;

    constexpr Floor compute_floor_down(Floor floor)
    {
      switch (floor) {
        case Floor::Underground:
          return Floor::Underground;
        case Floor::Ground:
          return Floor::Underground;
        case Floor::Upstairs:
          return Floor::Ground;
      }

      assert(false);
      return Floor::Ground;
    }

    constexpr Floor compute_floor_up(Floor floor)
    {
      switch (floor) {
        case Floor::Underground:
          return Floor::Ground;
        case Floor::Ground:
          return Floor::Upstairs;
        case Floor::Upstairs:
          return Floor::Upstairs;
      }

      assert(false);
      return Floor::Ground;
    }

  }

  WorldModel::WorldModel(gf::Random* random)
  : m_random(random)
  {
  }

  void WorldModel::bind(WorldGenerationAnalysis& analysis)
  {
    analysis.set_step(WorldGenerationStep::Data);
    state.bind(data);
    runtime.bind(data, state, m_random, analysis);
  }

  void WorldModel::update(gf::Time time)
  {
    if (m_phase == Phase::Cooldown) {
      m_cooldown += time;

      if (m_cooldown > Cooldown) {
        m_cooldown -= Cooldown;
        m_phase = Phase::Running;
      }

      return;
    }

    update_date();

    const gf::RectI view = runtime.compute_view();
    bool need_cooldown = false;

    while (state.current_date == state.scheduler.queue.top().date) {
      if (state.scheduler.is_hero_turn()) {
        if (update_hero()) {
          need_cooldown = true;
        }

        break;
      }

      const Task& current_task = state.scheduler.queue.top();

      if (current_task.type == TaskType::Actor) {
        assert(current_task.index < state.actors.size());
        gf::Log::debug("[SCHEDULER] {}: Update actor {}", state.current_date.to_string(), current_task.index);
        ActorState& actor = state.actors[current_task.index];

        const Action action = select_behavior(*this, actor, m_random);
        const ActionResult result = compute_action(*this, actor, action);

        if (result == ActionResult::Success && view.contains(actor.position)) {
          need_cooldown = true;
        }

        continue;
      }

      if (current_task.type == TaskType::Train) {
        assert(current_task.index < state.network.trains.size());
        gf::Log::debug("[SCHEDULER] {}: Update train {}", state.current_date.to_string(), current_task.index);

        if (update_train(state.network.trains[current_task.index], current_task.index)) {
          need_cooldown = true;
        }

        continue;
      }
    }

    if (need_cooldown) {
      m_phase = Phase::Cooldown;
    }
  }

  uint32_t WorldModel::index_of(ActorState& actor) const
  {
    const std::ptrdiff_t offset = &actor - state.actors.data();
    assert(offset >= 0);
    return static_cast<uint32_t>(offset);
  }

  bool WorldModel::is_walkable(Floor floor, gf::Vec2I position) const
  {
    const FloorMap& floor_map = runtime.map.from_floor(floor);

    if (!floor_map.background.valid(position)) {
      return false;
    }

    if (!floor_map.background(position).walkable()) {
      return false;
    }

    assert(floor_map.reverse.valid(position));
    const ReverseMapCell& cell = floor_map.reverse(position);

    if (!cell.empty()) {
      return false;
    }

    return true;
  }

  void WorldModel::update_date()
  {
    state.current_date = state.scheduler.queue.top().date;
  }

  void WorldModel::update_current_task_in_queue(uint16_t seconds)
  {
    Task task = state.scheduler.queue.top();
    state.scheduler.queue.pop();
    task.date.add_seconds(seconds);

    gf::Log::debug("\tNext turn: {}", task.date.to_string());

    state.scheduler.queue.push(task);
  }

  bool WorldModel::update_hero()
  {
    if (!runtime.hero.moves.empty()) {
      runtime.hero.action = make_action<MoveAction>(runtime.hero.moves.back() - state.hero().position);
      runtime.hero.moves.pop_back();
    }

    if (runtime.hero.action.type() == ActionType::None) {
      return false;
    }

    gf::Log::debug("[SCHEDULER] {}: Update hero", state.current_date.to_string());

    ActorState& hero = state.hero();
    const ActionResult result = compute_action(*this, hero, runtime.hero.action);

    if (runtime.hero.action.type() == ActionType::Move) {
      if (result == ActionResult::Success) {
        BackgroundMap& state_map = state.map.from_floor(hero.floor);
        const std::vector<gf::Vec2I> explored = compute_hero_fov(hero.position, state_map);

        FloorMap& runtime_map = runtime.map.from_floor(hero.floor);
        runtime_map.update_minimap_explored(explored);
      } else {
        runtime.hero.moves.clear();
      }

    }

    if (check_actor_position(hero)) {
      runtime.hero.moves.clear();
    }

    runtime.hero.action = {};
    return result == ActionResult::Success;
  }

  bool WorldModel::check_actor_position(ActorState& actor)
  {
    MapCellDecoration decoration = MapCellDecoration::None;

    switch (actor.floor) {
      case Floor::Underground:
        decoration = state.map.underground(actor.position).decoration;
        break;
      case Floor::Ground:
        decoration = state.map.ground(actor.position).decoration;
        break;
      case Floor::Upstairs:
        assert(false); // TODO: upstairs
        break;
    }

    switch (decoration) {
      case MapCellDecoration::FloorDown:
        return change_floor(actor, compute_floor_down(actor.floor));
      case MapCellDecoration::FloorUp:
        return change_floor(actor, compute_floor_up(actor.floor));
      default:
        break;
    }

    return false;
  }

  bool WorldModel::change_floor(ActorState& actor, Floor new_floor)
  {
    gf::Log::debug("Want to change floor: {} -> {}", int(actor.floor), int(new_floor));

    if (actor.floor == new_floor) {
      return false;
    }

    if (actor.feature.type() == ActorType::Human && actor.feature.from<ActorType::Human>().mounting != NoIndex) {
      // actor is mounted, no floor change possible
      return false;
    }

    FloorMap& old_floor_map = runtime.map.from_floor(actor.floor);
    FloorMap& new_floor_map = runtime.map.from_floor(new_floor);

    ReverseMapCell& old_map_cell = old_floor_map.reverse(actor.position);
    ReverseMapCell& new_map_cell = new_floor_map.reverse(actor.position);

    if (new_map_cell.actor_index != NoIndex) {
      return false;
    }

    gf::Log::debug("Change floor!");

    std::swap(old_map_cell.actor_index, new_map_cell.actor_index);
    actor.floor = new_floor;

    // update fov for hero

    if (&actor == &state.hero()) {
      BackgroundMap& map = state.map.from_floor(new_floor);
      const std::vector<gf::Vec2I> explored = compute_hero_fov(actor.position, map);
      new_floor_map.update_minimap_explored(explored);
    }

    return true;
  }

  bool WorldModel::update_train(TrainState& train, uint32_t train_index)
  {
    runtime.set_reverse_train(train, NoIndex);

    const uint32_t new_index = runtime.network.prev_position(train.railway_index);
    assert(new_index < runtime.network.railway.size());
    const gf::Vec2I new_position = runtime.network.railway[new_index];

    train.railway_index = new_index;

    runtime.set_reverse_train(train, train_index);

    if (auto iterator = std::find_if(state.network.stations.begin(), state.network.stations.end(), [new_index](const StationState& station) {
      return station.index == new_index;
    }); iterator != state.network.stations.end()) {
      update_current_task_in_queue(iterator->stop_time);
    } else {
      update_current_task_in_queue(TrainTime);
    }

    const int32_t distance_to_hero = gf::chebyshev_distance(new_position, state.hero().position);

    if (distance_to_hero > IdleDistance) {
      return false; // do not cooldown
    }

    return true;
  }

}
