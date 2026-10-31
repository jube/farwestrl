#include "WorldGeneration_Places.h"
#include "MapCell.h"
#include "gf2/core/Range.h"

#include <algorithm>

#include <gf2/core/Direction.h>

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

    void add_building_to_town(const Building& building, gf::Vec2I position, Place& town)
    {
      position += building.offset;

      // surrounding wall

      for (int i = 0; i < TownBuildingSize; ++i) {
        town.map(position + gf::dirx(i)).decoration = MapCellDecoration::Wall;
        town.map(position + gf::diry(i)).decoration = MapCellDecoration::Wall;
        town.map(position + gf::dirx(i) + gf::diry(TownBuildingSize - 1)).decoration = MapCellDecoration::Wall;
        town.map(position + gf::diry(i) + gf::dirx(TownBuildingSize - 1)).decoration = MapCellDecoration::Wall;
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
