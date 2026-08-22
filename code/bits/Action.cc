#include "Action.h"

#include "ActorState.h"
#include "ItemData.h"
#include "ItemState.h"
#include "MapCell.h"
#include "NetworkState.h"
#include "Times.h"
#include "WorldModel.h"
#include <cstdint>

namespace fw {

  namespace {
    /*
     * Helpers
     */

    void apply_move(WorldModel& model, Location& location, gf::Vec2I position) {
      assert(gf::chebyshev_distance(location.position, position) < 2);

      if (location.position == position) {
        return;
      }

      FloorMap& floor_map = model.runtime.map.from_floor(location.floor);

      assert(floor_map.reverse.valid(location.position));
      ReverseMapCell& old_reverse_cell = floor_map.reverse(location.position);

      // gf::Log::debug("Actor: index = {}, reverse.actor_index = {}", model.index_of(actor), old_reverse_cell.actor_index);

      assert(old_reverse_cell.actor_index < model.state.actors.size());

      assert(floor_map.reverse.valid(position));
      ReverseMapCell& new_reverse_cell = floor_map.reverse(position);
      assert(floor_map.reverse(position).actor_index == NoIndex);

      location.position = position;
      std::swap(old_reverse_cell.actor_index, new_reverse_cell.actor_index);

      assert(model.check());
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

    void apply_change_floor(WorldModel& model, Location& location, Floor new_floor)
    {
      gf::Log::debug("Want to change floor: {} -> {}", int(location.floor), int(new_floor));

      if (location.floor == new_floor) {
        return;
      }

      // if (actor.component.type() == ActorType::Human && actor.component.from<ActorType::Human>().mounting != NoIndex) {
      //   // actor is mounted, no floor change possible
      //   return;
      // }

      FloorMap& old_floor_map = model.runtime.map.from_floor(location.floor);
      FloorMap& new_floor_map = model.runtime.map.from_floor(new_floor);

      ReverseMapCell& old_map_cell = old_floor_map.reverse(location.position);
      ReverseMapCell& new_map_cell = new_floor_map.reverse(location.position);

      if (new_map_cell.actor_index != NoIndex) {
        // there is already an actor on the target cell
        return;
      }

      gf::Log::debug("Change floor!");

      std::swap(old_map_cell.actor_index, new_map_cell.actor_index);
      location.floor = new_floor;
    }

    ActionResult compute_move_human_action(WorldModel& model, HumanComponent& component, gf::Vec2I position)
    {
      if (!model.is_walkable(component.location.floor, position)) {
        return ActionResult::Failure;
      }

      const int32_t move_length = gf::manhattan_length(component.location.position - position);
      const uint32_t mount_index = component.mounting;

      if (mount_index == NoIndex) {
        // the human is not mouting an animal

        apply_move(model, component.location, position);

        if (move_length == 2) {
          model.update_current_task_in_queue(DiagonalWalkTime);
        } else if (move_length == 1) {
          model.update_current_task_in_queue(StraightWalkTime);
        }
      } else {
        // the humain is mouting an animal

        ActorState& mount = model.state.actors[mount_index];

        assert(mount.component.type() == ActorType::Animal);
        AnimalComponent& mount_component = mount.component.from<ActorType::Animal>();

        apply_move(model, mount_component.location, position);
        component.location.position = position;

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

    void maybe_change_floor(WorldModel& model, Location& location)
    {
      const MapCellDecoration decoration = model.state.map.from_floor(location.floor)(location.position).decoration;

      switch (decoration) {
        case MapCellDecoration::FloorDown:
          apply_change_floor(model, location, compute_floor_down(location.floor));
          break;
        case MapCellDecoration::FloorUp:
          apply_change_floor(model, location, compute_floor_up(location.floor));
          break;
        default:
          break;
      }
    }

    ActionResult compute_move_action(WorldModel& model, ActorState& actor, const MoveAction& action)
    {
      const gf::Vec2I displacement = gf::clamp(action.displacement, -1, +1);

      switch (actor.component.type()) {
        case ActorType::None:
        case ActorType::Group:
        case ActorType::Train:
          break;
        case ActorType::Human:
        {
          HumanComponent& component = actor.component.from<ActorType::Human>();
          const gf::Vec2I new_position = component.location.position + displacement;
          const ActionResult result = compute_move_human_action(model, component, new_position);

          if (result == ActionResult::Success) {
            maybe_change_floor(model, component.location);
          }

          return result;
        }
        case ActorType::Animal:
        {
          [[maybe_unused]] AnimalComponent& component = actor.component.from<ActorType::Animal>();
          // TODO
          return ActionResult::Success;
        }
      }

      return ActionResult::Failure;
    }

    // Mount

    ActionResult compute_mount_action(WorldModel& model, ActorState& actor, [[maybe_unused]] const MountAction& action)
    {
      assert(actor.component.type() == ActorType::Human);
      HumanComponent& human_component = actor.component.from<ActorType::Human>();

      FloorMap& floor_map = model.runtime.map.from_floor(human_component.location.floor);
      ReverseMapCell& actor_cell = floor_map.reverse(human_component.location.position);

      if (human_component.mounting != NoIndex) {
        // the hero is already mouting an animal
        return ActionResult::Failure;
      }

      gf::Log::debug("The hero is not mouting an animal.");

      std::vector<uint32_t> actor_indices;

      for (const gf::Vec2I neighbor : floor_map.reverse.compute_4_neighbors_range(human_component.location.position)) {
        const ReverseMapCell& cell = floor_map.reverse(neighbor);

        if (cell.actor_index == NoIndex) {
          // not actor on this cell
          continue;
        }

        gf::Log::debug("There is an actor next to the hero: {}", cell.actor_index);

        actor_indices.push_back(cell.actor_index);
      }

      for (const uint32_t animal_index : actor_indices) {
        assert(animal_index < model.state.actors.size());
        ActorState& animal = model.state.actors[animal_index];

        if (animal.component.type() != ActorType::Animal) {
          // it's not an animal
          continue;
        }

        gf::Log::debug("The actor {} is an animal", animal_index);

        const AnimalElement& animal_element = animal.data->element.from<ActorType::Animal>();

        if (!animal_element.can_be_mounted) {
          // the animal cannot be mounted
          continue;
        }

        gf::Log::debug("The actor {} can be mounted", animal_index);

        AnimalComponent& animal_component = animal.component.from<ActorType::Animal>();

        if (animal_component.mounted_by != NoIndex) {
          // the animal is already mounted
          continue;
        }

        gf::Log::debug("Mount!");

        human_component.mounting = animal_index;
        human_component.location.position = animal_component.location.position;

        std::swap(animal_component.mounted_by, actor_cell.actor_index);
        model.update_current_task_in_queue(MountTime);
        return ActionResult::Success;
      }

      return ActionResult::Failure;
    }

    // Dismount

    ActionResult compute_dismount_action(WorldModel& model, ActorState& actor, [[maybe_unused]] const DismountAction& action)
    {
      assert(actor.component.type() == ActorType::Human);
      HumanComponent& human_component = actor.component.from<ActorType::Human>();

      FloorMap& floor_map = model.runtime.map.from_floor(human_component.location.floor);

      if (human_component.mounting == NoIndex) {
        // the actor is not mouting an animal
        return ActionResult::Failure;
      }

      gf::Log::debug("The actor is mouting an animal.");

      std::optional<gf::Vec2I> maybe_position;

      for (const gf::Vec2I neighbor : floor_map.reverse.compute_4_neighbors_range(human_component.location.position)) {
        if (!model.is_walkable(human_component.location.floor, neighbor)) {
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

      human_component.location.position = maybe_position.value();
      ReverseMapCell& actor_cell = floor_map.reverse(human_component.location.position);
      assert(actor_cell.actor_index == NoIndex);
      actor_cell.actor_index = model.index_of(actor);

      ActorState& mount = model.state.actors[human_component.mounting];
      assert(mount.component.type() == ActorType::Animal);
      mount.component.from<ActorType::Animal>().mounted_by = NoIndex;

      human_component.mounting = NoIndex;
      model.update_current_task_in_queue(MountTime);
      return ActionResult::Success;
    }

    // Reload

    ActionResult compute_reload_action(WorldModel& model, ActorState& actor, [[maybe_unused]] const ReloadAction& action)
    {
      assert(actor.component.type() == ActorType::Human);
      HumanComponent& human_component = actor.component.from<ActorType::Human>();

      if (!human_component.weapon.data || !human_component.projectile.data) {
        // the actor has no weapon or no projectile
        return ActionResult::Failure;
      }

      if (human_component.weapon.data->element.type() != ItemType::DistanceWeapon) {
        // the weapon is not a distance weapon
        return ActionResult::Failure;
      }

      if (human_component.projectile.data->element.type() != ItemType::Projectile) {
        // the projectile is not really projectile
        return ActionResult::Failure;
      }

      const DistanceWeaponElement& weapon_element = human_component.weapon.data->element.from<ItemType::DistanceWeapon>();
      DistanceWeaponComponent& weapon_component = human_component.weapon.component.from<ItemType::DistanceWeapon>();

      const ProjectileElement& projectile_element = human_component.projectile.data->element.from<ItemType::Projectile>();

      if (weapon_element.projectile != projectile_element.kind) {
        // the projectile kind differs
        return ActionResult::Failure;
      }

      const int16_t needed_projectiles = weapon_element.capacity - weapon_component.projectiles;
      const int16_t loaded_projectiles = std::min(needed_projectiles, human_component.projectile.count);

      if (loaded_projectiles > 0) {
        // there are enough projectiles
        weapon_component.projectiles += loaded_projectiles;
        human_component.projectile.count -= loaded_projectiles;

        model.state.add_message(fmt::format("<style=character>{}</> reloads its weapon with {} projectiles.", actor.component.from<ActorType::Human>().name, loaded_projectiles));

        model.update_current_task_in_queue(weapon_element.reload_time);
        return ActionResult::Success;
      }

      return ActionResult::Failure;

    }

    // Graze

    ActionResult compute_graze_action(WorldModel& model, ActorState& actor, const GrazeAction& action)
    {
      assert(actor.component.type() == ActorType::Animal);
      AnimalComponent& component = actor.component.from<ActorType::Animal>();

      const gf::Vec2I displacement = gf::clamp(action.displacement, -1, +1);
      const gf::Vec2I new_position = component.location.position + displacement;

      if (model.is_walkable(component.location.floor, new_position)) {
        apply_move(model, component.location, new_position);
      }

      model.update_current_task_in_queue(GrazeTime);
      return ActionResult::Success;
    }

    // Wander

    ActionResult compute_wander_move(WorldModel& model, Location& location, gf::Vec2I new_position)
    {
      if (!model.is_walkable(location.floor, new_position)) {
        model.update_current_task_in_queue(WanderIdleTime);
        return ActionResult::Failure;
      }

      apply_move(model, location, new_position);
      model.update_current_task_in_queue(WanderTime);
      return ActionResult::Success;
    }

    ActionResult compute_wander_action(WorldModel& model, ActorState& actor, const WanderAction& action)
    {
      const gf::Vec2I displacement = gf::clamp(action.displacement, -1, +1);

      switch (actor.component.type()) {
        case ActorType::None:
        case ActorType::Group:
        case ActorType::Train:
          break;
        case ActorType::Human:
        {
          HumanComponent& component = actor.component.from<ActorType::Human>();
          const gf::Vec2I new_position = component.location.position + displacement;
          return compute_wander_move(model, component.location, new_position);
        }
        case ActorType::Animal:
        {
          AnimalComponent& component = actor.component.from<ActorType::Animal>();
          const gf::Vec2I new_position = component.location.position + displacement;

          const AnimalElement& element = actor.data->element.from<ActorType::Animal>();

          if (element.biome != model.state.map.from_floor(component.location.floor)(new_position).region) {
            // the new position is not on the preferred biome of the animal
            model.update_current_task_in_queue(WanderIdleTime);
            return ActionResult::Failure;
          }

          return compute_wander_move(model, component.location, new_position);
        }
      }

      return ActionResult::Failure;
    }

    // Cruise

    ActionResult compute_cruise_action(WorldModel& model, ActorState& actor, [[maybe_unused]] const CruiseAction& action)
    {
      assert(actor.component.type() == ActorType::Train);
      TrainComponent& component = actor.component.from<ActorType::Train>();
      const uint32_t train_index = model.index_of(actor);

      model.runtime.set_reverse_train(component.railway_index, NoIndex);

      const uint32_t new_index = model.runtime.network.prev_position(component.railway_index);
      assert(new_index < model.runtime.network.railway.size());
      // const gf::Vec2I new_position = model.runtime.network.railway[new_index];
      component.railway_index = new_index;

      model.runtime.set_reverse_train(component.railway_index, train_index);

      if (auto iterator = std::ranges::find(model.state.network.stations, new_index, &StationState::index); iterator != model.state.network.stations.end()) {
        model.update_current_task_in_queue(iterator->stop_time);
      } else {
        model.update_current_task_in_queue(TrainTime);
      }

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
      case ActionType::Wander:
        return compute_wander_action(model, actor, action.from<ActionType::Wander>());
      case ActionType::Cruise:
        return compute_cruise_action(model, actor, action.from<ActionType::Cruise>());
      }

    return ActionResult::Failure;
  }

}
