#include "Action.h"

#include "ActorState.h"
#include "MapCell.h"
#include "Times.h"
#include "WorldModel.h"

namespace fw {

  namespace {
    /*
     * Helpers
     */

    void apply_move(WorldModel& model, ActorState& actor, gf::Vec2I position) {
      assert(gf::chebyshev_distance(actor.position, position) < 2);

      if (actor.position == position) {
        return;
      }

      FloorMap& floor_map = model.runtime.map.from_floor(actor.floor);

      assert(floor_map.reverse.valid(actor.position));
      ReverseMapCell& old_reverse_cell = floor_map.reverse(actor.position);
      assert(old_reverse_cell.actor_index < model.state.actors.size());
      assert(&actor == &model.state.actors[old_reverse_cell.actor_index]);

      assert(floor_map.reverse.valid(position));
      ReverseMapCell& new_reverse_cell = floor_map.reverse(position);
      assert(floor_map.reverse(position).actor_index == NoIndex);

      actor.position = position;
      std::swap(old_reverse_cell.actor_index, new_reverse_cell.actor_index);
    }

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

    void apply_change_floor(WorldModel& model, ActorState& actor, Floor new_floor)
    {
      gf::Log::debug("Want to change floor: {} -> {}", int(actor.floor), int(new_floor));

      if (actor.floor == new_floor) {
        return;
      }

      if (actor.feature.type() == ActorType::Human && actor.feature.from<ActorType::Human>().mounting != NoIndex) {
        // actor is mounted, no floor change possible
        return;
      }

      FloorMap& old_floor_map = model.runtime.map.from_floor(actor.floor);
      FloorMap& new_floor_map = model.runtime.map.from_floor(new_floor);

      ReverseMapCell& old_map_cell = old_floor_map.reverse(actor.position);
      ReverseMapCell& new_map_cell = new_floor_map.reverse(actor.position);

      if (new_map_cell.actor_index != NoIndex) {
        // there is already an actor on the target cell
        return;
      }

      gf::Log::debug("Change floor!");

      std::swap(old_map_cell.actor_index, new_map_cell.actor_index);
      actor.floor = new_floor;
    }

    ActionResult compute_move_human_action(WorldModel& model, ActorState& actor, gf::Vec2I position)
    {
      assert(actor.feature.type() == ActorType::Human);

      if (!model.is_walkable(actor.floor, position)) {
        return ActionResult::Failure;
      }

      const int32_t move_length = gf::manhattan_length(actor.position - position);
      const uint32_t mount_index = actor.feature.from<ActorType::Human>().mounting;

      if (mount_index == NoIndex) {
        // the human is not mouting an animal

        apply_move(model, actor, position);

        if (move_length == 2) {
          model.update_current_task_in_queue(DiagonalWalkTime);
        } else if (move_length == 1) {
          model.update_current_task_in_queue(StraightWalkTime);
        }
      } else {
        // the humain is mouting an animal

        ActorState& mount = model.state.actors[mount_index];
        apply_move(model, mount, position);
        actor.position = position;

        if (move_length == 2) {
          model.update_current_task_in_queue(DiagonalWalkTime); // TODO: change the time according to mount
        } else if (move_length == 1) {
          model.update_current_task_in_queue(StraightWalkTime);
        }
      }

      return ActionResult::Success;
    }


    /*
     * Actions
     */

    // Idle

    ActionResult compute_idle_action(WorldModel& model, [[maybe_unused]] ActorState& actor, const IdleAction& action)
    {
      model.update_current_task_in_queue(action.time);
      return ActionResult::Success;
    }

    // Move

    ActionResult compute_move_action(WorldModel& model, ActorState& actor, const MoveAction& action)
    {
      const gf::Vec2I displacement = gf::clamp(action.displacement, -1, +1);
      const gf::Vec2I new_position = actor.position + displacement;

      ActionResult result = ActionResult::Failure;

      if (actor.feature.type() == ActorType::Human) {
        result = compute_move_human_action(model, actor, new_position);
      }

      // TODO: animals and others

      if (result == ActionResult::Success) {
        const MapCellDecoration decoration = model.state.map.from_floor(actor.floor)(actor.position).decoration;

        switch (decoration) {
          case MapCellDecoration::FloorDown:
            apply_change_floor(model, actor, compute_floor_down(actor.floor));
            break;
          case MapCellDecoration::FloorUp:
            apply_change_floor(model, actor, compute_floor_up(actor.floor));
            break;
          default:
            break;
        }
      }

      return result;
    }

    // Mount

    ActionResult compute_mount_action(WorldModel& model, ActorState& actor, [[maybe_unused]] const MountAction& action)
    {
      assert(actor.feature.type() == ActorType::Human);

      FloorMap& floor_map = model.runtime.map.from_floor(actor.floor);

      HumanFeature& actor_feature = actor.feature.from<ActorType::Human>();
      ReverseMapCell& actor_cell = floor_map.reverse(actor.position);

      if (actor_feature.mounting != NoIndex) {
        // the hero is already mouting an animal
        return ActionResult::Failure;
      }

      gf::Log::debug("The hero is not mouting an animal.");

      std::vector<uint32_t> actor_indices;

      for (const gf::Vec2I neighbor : floor_map.reverse.compute_4_neighbors_range(actor.position)) {
        const ReverseMapCell& cell = floor_map.reverse(neighbor);

        if (cell.actor_index == NoIndex) {
          // not actor on this cell
          continue;
        }

        gf::Log::debug("There is an actor next to the hero: {}", cell.actor_index);

        actor_indices.push_back(cell.actor_index);
      }

      for (const uint32_t animal_actor_index : actor_indices) {
        assert(animal_actor_index < model.state.actors.size());
        ActorState& animal_actor = model.state.actors[animal_actor_index];

        if (animal_actor.feature.type() != ActorType::Animal) {
          // it's not an animal
          continue;
        }

        gf::Log::debug("The actor {} is an animal", animal_actor_index);

        const AnimalDataFeature& animal_data_feature = animal_actor.data->feature.from<ActorType::Animal>();

        if (!animal_data_feature.can_be_mounted) {
          // the animal cannot be mounted
          continue;
        }

        gf::Log::debug("The actor {} can be mounted", animal_actor_index);

        AnimalFeature& animal_feature = animal_actor.feature.from<ActorType::Animal>();

        if (animal_feature.mounted_by != NoIndex) {
          // the animal is already mounted
          continue;
        }

        gf::Log::debug("Mount!");

        actor_feature.mounting = animal_actor_index;
        actor.position = animal_actor.position;

        std::swap(animal_feature.mounted_by, actor_cell.actor_index);
        model.update_current_task_in_queue(MountTime);
        return ActionResult::Success;
      }

      return ActionResult::Failure;
    }

    // Dismount

    ActionResult compute_dismount_action(WorldModel& model, ActorState& actor, [[maybe_unused]] const DismountAction& action)
    {
      assert(actor.feature.type() == ActorType::Human);

      FloorMap& floor_map = model.runtime.map.from_floor(actor.floor);

      HumanFeature& actor_feature = actor.feature.from<ActorType::Human>();

      if (actor_feature.mounting == NoIndex) {
        // the actor is not mouting an animal
        return ActionResult::Failure;
      }

      gf::Log::debug("The actor is mouting an animal.");

      std::optional<gf::Vec2I> maybe_position;

      for (const gf::Vec2I neighbor : floor_map.reverse.compute_4_neighbors_range(actor.position)) {
        if (!model.is_walkable(actor.floor, neighbor)) {
          continue;
        }

        gf::Log::debug("There is an empty place next to the actor");

        maybe_position = neighbor;
        break;
      }

      if (!maybe_position) {
        gf::Log::debug("There is no empty place next to the actor");
        return ActionResult::Failure;
      }

      actor.position = maybe_position.value();
      ReverseMapCell& actor_cell = floor_map.reverse(actor.position);
      assert(actor_cell.actor_index == NoIndex);
      actor_cell.actor_index = model.index_of(actor);

      ActorState& mount = model.state.actors[actor_feature.mounting];
      assert(mount.feature.type() == ActorType::Animal);
      mount.feature.from<ActorType::Animal>().mounted_by = NoIndex;

      actor_feature.mounting = NoIndex;
      model.update_current_task_in_queue(MountTime);
      return ActionResult::Success;
    }

    // Reload

    ActionResult compute_reload_action(WorldModel& model, ActorState& actor, [[maybe_unused]] const ReloadAction& action)
    {
      assert(actor.feature.type() == ActorType::Human);

      if (!actor.weapon.data || !actor.ammunition.data) {
        // the actor has no weapon or no ammunition
        return ActionResult::Failure;
      }

      if (actor.weapon.data->feature.type() != ItemType::Firearm) {
        // the weapon is not a firearm
        return ActionResult::Failure;
      }

      if (actor.ammunition.data->feature.type() != ItemType::Ammunition) {
        // the ammunition is not really ammunition
        return ActionResult::Failure;
      }

      const FirearmDataFeature& firearm = actor.weapon.data->feature.from<ItemType::Firearm>();
      const AmmunitionDataFeature& ammunition = actor.ammunition.data->feature.from<ItemType::Ammunition>();

      if (firearm.caliber != ammunition.caliber) {
        // the caliber differs
        return ActionResult::Failure;
      }

      const int16_t needed_cartridges = firearm.capacity - actor.weapon.cartridges;
      const int16_t loaded_cartriges = std::min(needed_cartridges, actor.ammunition.count);

      if (loaded_cartriges > 0) {
        // there are enough cartridges
        actor.weapon.cartridges += loaded_cartriges;
        actor.ammunition.count -= loaded_cartriges;

        model.state.add_message(fmt::format("<style=character>{}</> reloads its weapon with {} cartridges.", actor.feature.from<ActorType::Human>().name, loaded_cartriges));

        model.update_current_task_in_queue(firearm.reload_time);
        return ActionResult::Success;
      }

      return ActionResult::Failure;

    }

    // Graze

    ActionResult compute_graze_action(WorldModel& model, ActorState& actor, const GrazeAction& action)
    {
      const gf::Vec2I displacement = gf::clamp(action.displacement, -1, +1);
      const gf::Vec2I new_position = actor.position + displacement;

      if (model.is_walkable(actor.floor, new_position)) {
        apply_move(model, actor, new_position);
      }

      model.update_current_task_in_queue(GrazeTime);
      return ActionResult::Success;
    }
  }

  ActionResult compute_action(WorldModel& model, ActorState& actor, const Action& action)
  {
    switch (action.type()) {
      case ActionType::None:
        assert(false);
        return ActionResult::Success;
      case ActionType::Idle:
        return compute_idle_action(model, actor, action.from<ActionType::Idle>());
      case ActionType::Move:
        return compute_move_action(model, actor, action.from<ActionType::Move>());
      case ActionType::Mount:
        return compute_mount_action(model, actor, action.from<ActionType::Mount>());
      case ActionType::Dismount:
        return compute_dismount_action(model, actor, action.from<ActionType::Dismount>());
      case ActionType::Reload:
        return compute_reload_action(model, actor, action.from<ActionType::Reload>());
      case ActionType::Graze:
        return compute_graze_action(model, actor, action.from<ActionType::Graze>());
    }

    return ActionResult::Failure;
  }

}
