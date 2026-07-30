#include "Action.h"

#include "ActorState.h"
#include "ItemData.h"
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

      // if (actor.feature.type() == ActorType::Human && actor.feature.from<ActorType::Human>().mounting != NoIndex) {
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

    ActionResult compute_move_human_action(WorldModel& model, HumanFeature& feature, gf::Vec2I position)
    {
      if (!model.is_walkable(feature.location.floor, position)) {
        return ActionResult::Failure;
      }

      const int32_t move_length = gf::manhattan_length(feature.location.position - position);
      const uint32_t mount_index = feature.mounting;

      if (mount_index == NoIndex) {
        // the human is not mouting an animal

        apply_move(model, feature.location, position);

        if (move_length == 2) {
          model.update_current_task_in_queue(DiagonalWalkTime);
        } else if (move_length == 1) {
          model.update_current_task_in_queue(StraightWalkTime);
        }
      } else {
        // the humain is mouting an animal

        ActorState& mount = model.state.actors[mount_index];

        assert(mount.feature.type() == ActorType::Animal);
        AnimalFeature& mount_feature = mount.feature.from<ActorType::Animal>();

        apply_move(model, mount_feature.location, position);
        feature.location.position = position;

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

      switch (actor.feature.type()) {
        case ActorType::None:
        case ActorType::Group:
        case ActorType::Train:
          break;
        case ActorType::Human:
        {
          HumanFeature& feature = actor.feature.from<ActorType::Human>();
          const gf::Vec2I new_position = feature.location.position + displacement;
          const ActionResult result = compute_move_human_action(model, feature, new_position);

          if (result == ActionResult::Success) {
            maybe_change_floor(model, feature.location);
          }

          return result;
        }
        case ActorType::Animal:
        {
          [[maybe_unused]] AnimalFeature& feature = actor.feature.from<ActorType::Animal>();
          // TODO
          return ActionResult::Success;
        }
      }

      return ActionResult::Failure;
    }

    // Mount

    ActionResult compute_mount_action(WorldModel& model, ActorState& actor, [[maybe_unused]] const MountAction& action)
    {
      assert(actor.feature.type() == ActorType::Human);
      HumanFeature& feature = actor.feature.from<ActorType::Human>();

      FloorMap& floor_map = model.runtime.map.from_floor(feature.location.floor);
      ReverseMapCell& actor_cell = floor_map.reverse(feature.location.position);

      if (feature.mounting != NoIndex) {
        // the hero is already mouting an animal
        return ActionResult::Failure;
      }

      gf::Log::debug("The hero is not mouting an animal.");

      std::vector<uint32_t> actor_indices;

      for (const gf::Vec2I neighbor : floor_map.reverse.compute_4_neighbors_range(feature.location.position)) {
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

        feature.mounting = animal_actor_index;
        feature.location.position = animal_feature.location.position;

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
      HumanFeature& feature = actor.feature.from<ActorType::Human>();

      FloorMap& floor_map = model.runtime.map.from_floor(feature.location.floor);

      if (feature.mounting == NoIndex) {
        // the actor is not mouting an animal
        return ActionResult::Failure;
      }

      gf::Log::debug("The actor is mouting an animal.");

      std::optional<gf::Vec2I> maybe_position;

      for (const gf::Vec2I neighbor : floor_map.reverse.compute_4_neighbors_range(feature.location.position)) {
        if (!model.is_walkable(feature.location.floor, neighbor)) {
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

      feature.location.position = maybe_position.value();
      ReverseMapCell& actor_cell = floor_map.reverse(feature.location.position);
      assert(actor_cell.actor_index == NoIndex);
      actor_cell.actor_index = model.index_of(actor);

      ActorState& mount = model.state.actors[feature.mounting];
      assert(mount.feature.type() == ActorType::Animal);
      mount.feature.from<ActorType::Animal>().mounted_by = NoIndex;

      feature.mounting = NoIndex;
      model.update_current_task_in_queue(MountTime);
      return ActionResult::Success;
    }

    // Reload

    ActionResult compute_reload_action(WorldModel& model, ActorState& actor, [[maybe_unused]] const ReloadAction& action)
    {
      assert(actor.feature.type() == ActorType::Human);
      HumanFeature& feature = actor.feature.from<ActorType::Human>();

      if (!feature.weapon.data || !feature.projectile.data) {
        // the actor has no weapon or no projectile
        return ActionResult::Failure;
      }

      if (feature.weapon.data->feature.type() != ItemType::DistanceWeapon) {
        // the weapon is not a distance weapon
        return ActionResult::Failure;
      }

      if (feature.projectile.data->feature.type() != ItemType::Projectile) {
        // the projectile is not really projectile
        return ActionResult::Failure;
      }

      const DistanceWeaponDataFeature& weapon = feature.weapon.data->feature.from<ItemType::DistanceWeapon>();
      const ProjectileDataFeature& projectile = feature.projectile.data->feature.from<ItemType::Projectile>();

      if (weapon.projectile != projectile.kind) {
        // the projectile kind differs
        return ActionResult::Failure;
      }

      const int16_t needed_projectiles = weapon.capacity - feature.weapon.projectiles;
      const int16_t loaded_projectiles = std::min(needed_projectiles, feature.projectile.count);

      if (loaded_projectiles > 0) {
        // there are enough projectiles
        feature.weapon.projectiles += loaded_projectiles;
        feature.projectile.count -= loaded_projectiles;

        model.state.add_message(fmt::format("<style=character>{}</> reloads its weapon with {} projectiles.", actor.feature.from<ActorType::Human>().name, loaded_projectiles));

        model.update_current_task_in_queue(weapon.reload_time);
        return ActionResult::Success;
      }

      return ActionResult::Failure;

    }

    // Graze

    ActionResult compute_graze_action(WorldModel& model, ActorState& actor, const GrazeAction& action)
    {
      assert(actor.feature.type() == ActorType::Animal);
      AnimalFeature& feature = actor.feature.from<ActorType::Animal>();

      const gf::Vec2I displacement = gf::clamp(action.displacement, -1, +1);
      const gf::Vec2I new_position = feature.location.position + displacement;

      if (model.is_walkable(feature.location.floor, new_position)) {
        apply_move(model, feature.location, new_position);
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

      switch (actor.feature.type()) {
        case ActorType::None:
        case ActorType::Group:
        case ActorType::Train:
          break;
        case ActorType::Human:
        {
          HumanFeature& feature = actor.feature.from<ActorType::Human>();
          const gf::Vec2I new_position = feature.location.position + displacement;
          return compute_wander_move(model, feature.location, new_position);
        }
        case ActorType::Animal:
        {
          AnimalFeature& feature = actor.feature.from<ActorType::Animal>();
          const gf::Vec2I new_position = feature.location.position + displacement;

          const AnimalDataFeature& data = actor.data->feature.from<ActorType::Animal>();

          if (data.biome != model.state.map.from_floor(feature.location.floor)(new_position).region) {
            // the new position is not on the preferred biome of the animal
            model.update_current_task_in_queue(WanderIdleTime);
            return ActionResult::Failure;
          }

          return compute_wander_move(model, feature.location, new_position);
        }
      }

      return ActionResult::Failure;
    }

    // Cruise

    ActionResult compute_cruise_action(WorldModel& model, ActorState& actor, [[maybe_unused]] const CruiseAction& action)
    {
      assert(actor.feature.type() == ActorType::Train);
      TrainFeature& feature = actor.feature.from<ActorType::Train>();
      const uint32_t train_index = model.index_of(actor);

      model.runtime.set_reverse_train(feature.railway_index, NoIndex);

      const uint32_t new_index = model.runtime.network.prev_position(feature.railway_index);
      assert(new_index < model.runtime.network.railway.size());
      // const gf::Vec2I new_position = model.runtime.network.railway[new_index];
      feature.railway_index = new_index;

      model.runtime.set_reverse_train(feature.railway_index, train_index);

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
