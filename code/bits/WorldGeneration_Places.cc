#include "WorldGeneration_Places.h"
#include "MapCell.h"

#include <algorithm>

#include <gf2/core/Direction.h>
#include <gf2/core/Range.h>

namespace fw {

  Place::Place(int32_t size)
  : map({ size, size })
  {
  }

  /*
   * TOWN
   */

  constexpr int32_t TownsBlockSize = 6;
  constexpr int32_t TownBuildingSize = 11;
  constexpr int32_t StreetSize = 3;

  constexpr int32_t TownRadius = (TownsBlockSize * TownBuildingSize + (TownsBlockSize - 1) * StreetSize - 1) / 2;
  constexpr int32_t TownDiameter = 2 * TownRadius + 1;

  constexpr gf::RectI Blocks = gf::RectI::from_size({ TownsBlockSize, TownsBlockSize });

  namespace {

    /*
     * TownMap
     */

    enum class BuildingType : uint8_t {
      Empty,
      None,
      // actual buildings
      Bank,
      Casino,
      Church,
      ClothShop,
      FoodShop,
      Hotel,
      House1,
      House2,
      House3,
      MarshalOffice,
      Restaurant,
      Saloon,
      School,
      WeaponShop,
    };

    struct Building {
      BuildingType type = BuildingType::Empty;
      gf::Direction direction = gf::Direction::Up;
      gf::Vec2I offset = { 0, 0 };
    };

    struct TownMap {
      int cross_west;
      int cross_east;
      int cross_north;
      int cross_south;
      std::array<std::array<Building, TownsBlockSize>, TownsBlockSize> blocks;

      template<typename Self>
      decltype(auto) operator()(this Self&& self, gf::Vec2I position) { return self.blocks[position.y][position.x]; }
    };

    bool is_along_streets(const TownMap& map, gf::Vec2I position)
    {
      return position.x == map.cross_west || position.x == map.cross_east || position.y == map.cross_north || position.y == map.cross_south;
    }

    void stack_building(TownMap& map, gf::Vec2I position, gf::Direction direction)
    {
      const gf::Vec2I step = gf::displacement(direction);
      gf::Vec2I current = position;

      while (Blocks.contains(current)) {
        assert(map(current).type != BuildingType::Empty);

        if (map(current).type != BuildingType::None) {
          std::swap(map(current), map(position));
          position += step;
        }

        current += step;
      }

    }

    gf::Direction compute_building_direction(const TownMap& map, gf::Vec2I position)
    {
      assert(is_along_streets(map, position));

      if (position.x == map.cross_west) {
        return gf::Direction::Right;
      }

      if (position.x == map.cross_east) {
        return gf::Direction::Left;
      }

      if (position.y == map.cross_north) {
        return gf::Direction::Down;
      }

      if (position.y == map.cross_north) {
        return gf::Direction::Up;
      }

      assert(false);
      return gf::Direction::Center;
    }

    gf::RectI compute_building_offset_range(const TownMap& map, gf::Vec2I position)
    {
      assert(is_along_streets(map, position));

      gf::RectI range = gf::RectI::from_center_size({ 0, 0 }, { 3, 3 });

      if (position.x == map.cross_west) {
        --range.extent.x;
      }

      if (position.x == map.cross_east) {
        ++range.offset.x;
      }

      if (position.y == map.cross_north) {
        --range.extent.y;
      }

      if (position.y == map.cross_north) {
        ++range.offset.y;
      }

      return range;
    }

    /*
     * An example of town map:
     *
     *     0   1   2   3   4   5
     *   +---+---+---+---+---+---+
     * 0 |   |   |   | # |   |   |
     *   +---+---+---+---+---+---+
     * 1 | # | # | # | # | # | # | N
     *   +---+---+---o---+---+---+
     * 2 |   | # | # | # | # |   | S
     *   +---+---+---+---+---+---+
     * 3 |   |   | # | # |   |   |
     *   +---+---+---+---+---+---+
     * 4 |   |   | # |   |   |   |
     *   +---+---+---+---+---+---+
     * 5 |   |   |   |   |   |   |
     *   +---+---+---+---+---+---+
     *             W   E
     */

    TownMap generate_town_map(gf::Random* random)
    {
      TownMap map;

      // determine the crossroad

      map.cross_west = random->compute_uniform_integer(1, 4);
      map.cross_east = map.cross_west + 1;
      map.cross_north = random->compute_uniform_integer(1, 4);
      map.cross_south = map.cross_north + 1;

      // put buildings randomly along the streets

      std::array<BuildingType, 20> buildings = {
        BuildingType::Bank,
        BuildingType::Casino,
        BuildingType::Church,
        BuildingType::ClothShop,
        BuildingType::FoodShop,
        BuildingType::Hotel,
        BuildingType::House1,
        BuildingType::House2,
        BuildingType::House3,
        BuildingType::MarshalOffice,
        BuildingType::Restaurant,
        BuildingType::Saloon,
        BuildingType::School,
        BuildingType::WeaponShop,

        BuildingType::None,
        BuildingType::None,
        BuildingType::None,
        BuildingType::None,
        BuildingType::None,
        BuildingType::None,
      };

      std::ranges::shuffle(buildings, random->engine());

      std::size_t building_index = 0;

      for (const gf::Vec2I position : gf::rectangle_range(Blocks)) {
        Building& building = map(position);

        if (is_along_streets(map, position) && building.type == BuildingType::Empty) {
          building.type = buildings[building_index++];
        }
      }

      // move buildings near the crossing

      stack_building(map, { map.cross_west, map.cross_north }, gf::Direction::Left);
      stack_building(map, { map.cross_west, map.cross_north }, gf::Direction::Up);
      stack_building(map, { map.cross_west, map.cross_south }, gf::Direction::Left);
      stack_building(map, { map.cross_west, map.cross_south }, gf::Direction::Down);
      stack_building(map, { map.cross_east, map.cross_north }, gf::Direction::Right);
      stack_building(map, { map.cross_east, map.cross_north }, gf::Direction::Up);
      stack_building(map, { map.cross_east, map.cross_south }, gf::Direction::Right);
      stack_building(map, { map.cross_east, map.cross_south }, gf::Direction::Down);

      // compute direction and offset

      for (const gf::Vec2I position : gf::rectangle_range(Blocks)) {
        Building& building = map(position);

        if (building.type == BuildingType::Empty || building.type == BuildingType::None) {
          continue;
        }

        building.direction = compute_building_direction(map, position);

        const gf::RectI range = compute_building_offset_range(map, position);
        building.offset = random->compute_position(range);
      }

      return map;
    }

    /*
     * Building
     */

    template<int32_t Size, typename Plan>
    char compute_generic_building_part(const Plan& plan, gf::Vec2I position, gf::Direction direction)
    {
      assert(0 <= position.x && position.x < Size);
      assert(0 <= position.y && position.y < Size);

      char picture = '#';

      switch (direction) {
        case gf::Direction::Up:
          picture = plan[position.y][position.x];
          break;
        case gf::Direction::Right:
          picture = plan[Size - position.x - 1][position.y];
          break;
        case gf::Direction::Down:
          picture = plan[Size - position.y - 1][Size - position.x - 1];
          break;
        case gf::Direction::Left:
          picture = plan[position.x][Size - position.y - 1];
          break;
        default:
          assert(false);
          break;
      }

      return picture;
    }

    using TownBuildingPlan = std::array<std::string_view, TownBuildingSize>;

    char compute_town_building_part(const TownBuildingPlan& plan, gf::Vec2I position, gf::Direction direction)
    {
      return compute_generic_building_part<TownBuildingSize>(plan, position, direction);
    }

    MapCellDecoration door_decoration(const TownBuildingPlan& plan, gf::Vec2I position, gf::Direction direction)
    {
      assert(compute_town_building_part(plan, position, direction) == 'D');
      assert(0 < position.x && position.x < TownBuildingSize - 1);
      assert(0 < position.y && position.y < TownBuildingSize - 1);

      auto check_wall = [&](gf::Vec2I offset) {
        return compute_town_building_part(plan, position + offset, direction) == '#';
      };

      if (check_wall(gf::dirx(+1))) {
        assert(check_wall(gf::dirx(-1)));
        return MapCellDecoration::HorizontalDoor;
      }

      assert(check_wall(gf::diry(+1)));
      assert(check_wall(gf::diry(-1)));
      return MapCellDecoration::VerticalDoor;
    }

    DoorState make_door(gf::Vec2I position, MapCellDecoration decoration)
    {
      const Location location = { .position = position, .floor = Floor::Ground };
      const DoorDirection direction = decoration == MapCellDecoration::HorizontalDoor ? DoorDirection::Horizontal : DoorDirection::Vertical;
      return { .location = location, .status = DoorStatus::Closed, .direction = direction };
    }

    constexpr TownBuildingPlan House2 = {{
      "#######W###",
      "#   #     #",
      "W   #     #",
      "#   #     #",
      "##D##D#####",
      "#     #   #",
      "#     D   #",
      "#     #   W",
      "#     #   #",
      "#   # #   #",
      "##W##D#####",
    }};

    void add_building_to_town(const Building& building, gf::Vec2I position, Place& town)
    {
      position += building.offset;

      const TownBuildingPlan& plan = House2; // TODO

      for (const gf::Vec2I offset : gf::rectangle_range(gf::RectI::from_size({ TownBuildingSize, TownBuildingSize }))) {
        MapCell& cell = town.map(position + offset);
        const char part = compute_town_building_part(plan, offset, building.direction);

        switch (part) {
          case '#': // wall
            cell.decoration = MapCellDecoration::Wall;
            break;
          case 'W': // window
            cell.decoration = MapCellDecoration::Window;
            break;
          case 'D': // door
            cell.decoration = door_decoration(plan, offset, building.direction);
            town.doors.emplace_back(make_door(position + offset, cell.decoration));
            break;
          case ' ': // nothing
            break;
          default:
            assert(false);
            break;
        }
      }

    }

  }

  Place generate_town(gf::Random* random)
  {
    TownMap map = generate_town_map(random);

    Place town(TownDiameter);

    for (const gf::Vec2I building_position : gf::rectangle_range(Blocks)) {
      const Building& building = map(building_position);

      if (building.type == BuildingType::Empty || building.type == BuildingType::None) {
        continue;
      }

      const gf::Vec2I town_position = building_position * (TownBuildingSize  + StreetSize);
      add_building_to_town(building, town_position, town);
    }

    return town;
  }


  /*
   * LOCALITY
   */

  constexpr int32_t LocalityRadius = 13;
  constexpr int32_t LocalityDiameter = 2 * LocalityRadius + 1;

  Place generate_locality(LocalityType type, gf::Random* random)
  {
    Place locality(LocalityDiameter);

    return locality;
  }

}
