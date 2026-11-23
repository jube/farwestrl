#include "WorldGeneration_Places.h"

#include <algorithm>

#include <gf2/core/Direction.h>
#include <gf2/core/Range.h>

#include "MapCell.h"
#include "ThingData.h"
#include "ThingGeneration.h"

namespace fw {

  Place::Place(int32_t size, MapCellBiome biome)
  : map({ size, size }, { .region = biome })
  {
  }

  /*
   * TOWN
   */

  constexpr gf::RectI TownMapSurface = gf::RectI::from_size({ TownsBlockSize, TownsBlockSize });
  constexpr gf::RectI BuildingSurface = gf::RectI::from_size({ TownBuildingSize, TownBuildingSize });
  constexpr gf::RectI TownSurface = gf::RectI::from_size({ TownDiameter, TownDiameter });

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

      while (TownMapSurface.contains(current)) {
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

      if (position.y == map.cross_north) {
        return gf::Direction::Down;
      }

      if (position.x == map.cross_west) {
        return gf::Direction::Right;
      }

      if (position.y == map.cross_south) {
        return gf::Direction::Up;
      }

      if (position.x == map.cross_east) {
        return gf::Direction::Left;
      }

      assert(false);
      return gf::Direction::Center;
    }

    gf::RectI compute_building_offset_range(const TownMap& map, gf::Vec2I position)
    {
      assert(is_along_streets(map, position));

      gf::RectI range = gf::RectI::from_position_size({ -1, -1 }, { 3, 3 });

      if (position.x == map.cross_west) {
        --range.extent.x;
      }

      if (position.x == map.cross_east) {
        --range.extent.x;
        ++range.offset.x;
      }

      if (position.y == map.cross_north) {
        --range.extent.y;
      }

      if (position.y == map.cross_south) {
        --range.extent.y;
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

      for (const gf::Vec2I position : gf::rectangle_range(TownMapSurface)) {
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

      for (const gf::Vec2I position : gf::rectangle_range(TownMapSurface)) {
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

    char rotate_part(char part, gf::Direction direction)
    {
      assert(direction != gf::Direction::Center);
      const int8_t index = static_cast<int8_t>(direction);
      assert(0 <= index && index < 4);

      switch (part) {
        case '.':
          return "^>.<"[index];
        case '<':
          return ">.<^"[index];
        case '^':
          return ".<^>"[index];
        case '>':
          return "<^>."[index];
        default:
          break;
      }

      return part;
    }

    template<int32_t Size, typename Plan>
    char compute_generic_building_part(const Plan& plan, gf::Vec2I position, gf::Direction direction)
    {
      assert(0 <= position.x && position.x < Size);
      assert(0 <= position.y && position.y < Size);

      char picture = '#';

      switch (direction) {
        case gf::Direction::Down:
          picture = plan[position.y][position.x];
          break;
        case gf::Direction::Left:
          picture = plan[Size - position.x - 1][position.y];
          break;
        case gf::Direction::Up:
          picture = plan[Size - position.y - 1][Size - position.x - 1];
          break;
        case gf::Direction::Right:
          picture = plan[position.x][Size - position.y - 1];
          break;
        default:
          assert(false);
          break;
      }

      return rotate_part(picture, direction);
    }

    using TownBuildingPlan = std::array<std::string_view, TownBuildingSize>;

    char compute_town_building_part(const TownBuildingPlan& plan, gf::Vec2I position, gf::Direction direction)
    {
      return compute_generic_building_part<TownBuildingSize>(plan, position, direction);
    }

    MapCellDecoration door_decoration(const TownBuildingPlan& plan, gf::Vec2I position, gf::Direction direction)
    {
      assert(compute_town_building_part(plan, position, direction) == 'D');
      assert(0 <= position.x && position.x < TownBuildingSize);
      assert(0 <= position.y && position.y < TownBuildingSize);

      auto check_wall = [&](gf::Vec2I offset) {
        return BuildingSurface.contains(position + offset) && compute_town_building_part(plan, position + offset, direction) == '#';
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

    gf::Direction thing_direction(const TownBuildingPlan& plan, gf::Vec2I position, gf::Direction direction)
    {
      if (compute_town_building_part(plan, position + gf::diry(-1), direction) == '.') {
        return gf::Direction::Up;
      }

      if (compute_town_building_part(plan, position + gf::diry(+1), direction) == '^') {
        return gf::Direction::Down;
      }

      if (compute_town_building_part(plan, position + gf::dirx(-1), direction) == '>') {
        return gf::Direction::Left;
      }

      if (compute_town_building_part(plan, position + gf::dirx(+1), direction) == '<') {
        return gf::Direction::Right;
      }

      assert(false);
      return gf::Direction::Center;
    }

    constexpr TownBuildingPlan Bank = {{
      "###########",
      "#         #",
      "#         #",
      "#####D#####",
      "#         #",
      "# TTTBTTT #",
      "#   $^$   #",
      "O   $ $   O",
      "#   $ $   #",
      "#   $ $   #",
      "##W##D##W##",
    }};

    constexpr TownBuildingPlan Casino = {{
      "#####O#####",
      "#       >M#",
      "# C< h  >M#",
      "# C<    >M#",
      "# C< c  >M#",
      "O       >M#",
      "# C< d  >M#",
      "# C<    >M#",
      "# C< s  >M#",
      "#       >M#",
      "##W##D##W##",
    }};

    constexpr TownBuildingPlan Church = {{
      "###########",
      "#         #",
      "# j TTT j #",
      "#         #",
      "# ... ... #",
      "# PPP PPP #",
      "# ... ... #",
      "# PPP PPP #",
      "# ... ... #",
      "# PPP PPP #",
      "##W##D##W##",
    }};

    constexpr TownBuildingPlan ClothShop = {{
      "###########",
      "#    #    #",
      "#    #    #",
      "#    #    #",
      "#         #",
      "#         #",
      "#  TTSTT  #",
      "W    ^    W",
      "#         #",
      "#         #",
      "##W##D##W##",
    }};

    constexpr TownBuildingPlan FoodShop = {{
      "###########",
      "#         #",
      "##        #",
      "#         #",
      "#         #",
      "W         W",
      "#         #",
      "#  .      #",
      "# TST     #",
      "#         #",
      "##W##D##W##",
    }};

    constexpr TownBuildingPlan Hotel = {{
      "###########",
      "# Z # # Z #",
      "# ^ # # ^ #",
      "O   # D   O",
      "#   D #   #",
      "##### #####",
      "#   #     #",
      "O   D   T #",
      "# . #  >H #",
      "# Z #   T #",
      "#####D#####",
    }};

    constexpr TownBuildingPlan House1 = {{
      "###O#######",
      "# Z  # #  #",
      "# ^  D #  #",
      "###### D  O",
      "#    D #  #",
      "O    # #  #",
      "###### ####",
      "#         #",
      "#         #",
      "#         #",
      "###O#D#O###",
    }};

    constexpr TownBuildingPlan House2 = {{
      "#######O###",
      "# Z #     #",
      "O ^ #     O",
      "#   #     #",
      "##D##D#####",
      "#     #   #",
      "#     D   #",
      "#     #   O",
      "#     #   #",
      "#   # #   #",
      "##O##D#####",
    }};

    constexpr TownBuildingPlan House3 = {{
      "###########",
      "#     # Z #",
      "O     # ^ #",
      "#     #   O",
      "###D###   #",
      "#     #   #",
      "#     ##D##",
      "O         #",
      "#         #",
      "#         #",
      "#####D##O##",
    }};

    constexpr TownBuildingPlan MarshalOffice = {{
      "###D#######",
      "#         #",
      "O         O",
      "#         #",
      "###D#######",
      "#      D  #",
      "W   TL #  #",
      "# T  ^ ####",
      "# L<   D  #",
      "#      #  #",
      "##W##D#####",
    }};

    constexpr TownBuildingPlan Saloon = {{
      "###########",
      "#       # #",
      "#TTATAT # #",
      "#  ^ ^    #",
      "#a< >a. >a#",
      "#T  aRa  T#",
      "Wa< ^a< >aW",
      "# >a. .a< #",
      "# aRa aRa #",
      "# ^a< >a^ #",
      "##W##D##W##",
    }};

    constexpr TownBuildingPlan Template = {{
      "###########",
      "#         #",
      "#         #",
      "#         #",
      "#         #",
      "#         #",
      "#         #",
      "#         #",
      "#         #",
      "#         #",
      "#####D#####",
    }};

    const TownBuildingPlan& compute_basic_town_building_plan(BuildingType type)
    {
      switch (type) {
      case BuildingType::Empty:
      case BuildingType::None:
        assert(false);
        break;
      case BuildingType::Bank:
        return Bank;
      case BuildingType::Casino:
        return Casino;
      case BuildingType::Church:
        return Church;
      case BuildingType::ClothShop:
        return ClothShop;
      case BuildingType::FoodShop:
        return FoodShop;
      case BuildingType::Hotel:
        return Hotel;
      case BuildingType::House1:
        return House1;
      case BuildingType::House2:
        return House2;
      case BuildingType::House3:
        return House3;
      case BuildingType::MarshalOffice:
        return MarshalOffice;
      case BuildingType::Restaurant:
        return Template; // TODO
      case BuildingType::Saloon:
        return Saloon;
      case BuildingType::School:
      case BuildingType::WeaponShop:
        break;
      }

      return Template;
    }

    void add_building_to_town(const Building& building, gf::Vec2I position, Place& town, const WorldData& data, gf::Random* random)
    {

      if (TownSurface.contains(position + building.offset) && TownSurface.contains(position + building.offset + TownBuildingSize - 1)) {
        position += building.offset;
      }

      assert(TownSurface.contains(position));
      assert(TownSurface.contains(position + TownBuildingSize - 1));

      const TownBuildingPlan& plan = compute_basic_town_building_plan(building.type);

      for (const gf::Vec2I offset : gf::rectangle_range(gf::RectI::from_size({ TownBuildingSize, TownBuildingSize }))) {
        const gf::Vec2I cell_position = position + offset;
        const Location cell_location = { .position = cell_position, .floor = Floor::Ground };

        auto add_thing = [&](ThingType type, ThingQuality quality) {
          const gf::Direction direction = thing_direction(plan, offset, building.direction);
          ThingState thing = generate_thing(type, quality, cell_location, direction, data, random);
          town.things.emplace_back(thing);
        };

        MapCell& cell = town.map(cell_position);
        cell.region = MapCellBiome::Building;

        const char part = compute_town_building_part(plan, offset, building.direction);

        switch (part) {
          case '#': // wall
            cell.decoration = MapCellDecoration::Wall;
            break;
          case 'A': // saloon desk (alcohol)
            add_thing(ThingType::SaloonDesk, ThingQuality::Good);
            break;
          case 'B': // bank desk
            add_thing(ThingType::BankDesk, ThingQuality::Good);
            break;
          case 'C': // blackjack table (card)
            add_thing(ThingType::BlackjackTable, ThingQuality::Best);
            break;
          case 'D': // door
            cell.decoration = door_decoration(plan, offset, building.direction);
            town.doors.emplace_back(make_door(cell_position, cell.decoration));
            break;
          case 'H': // hotel desk
            add_thing(ThingType::HotelDesk, ThingQuality::Good);
            break;
          case 'L': // marshal desk (law)
            add_thing(ThingType::MarshalDesk, ThingQuality::Good);
            break;
          case 'M': // slot machines
            add_thing(ThingType::SlotMachine, ThingQuality::Best);
            break;
          case 'O': // opaque window
            cell.decoration = MapCellDecoration::OpaqueWindow;
            break;
          case 'P': // prayer chair
            add_thing(ThingType::PrayerChair, ThingQuality::Good);
            break;
          case 'R': // round table
            cell.decoration = MapCellDecoration::RoundTable;
            break;
          case 'S': // shop desk
            add_thing(ThingType::ShopDesk, ThingQuality::Good);
            break;
          case 'T': // table
            cell.decoration = MapCellDecoration::Table;
            break;
          case 'W': // window
            cell.decoration = MapCellDecoration::Window;
            break;
          case 'Z': // bed (zzzz)
            add_thing(ThingType::Bed, ThingQuality::Good);
            break;
          case '$': // dollar
            cell.decoration = MapCellDecoration::Dollar;
            break;
          case 'a': // saloon chair (alcohol)
            add_thing(ThingType::SaloonChair, ThingQuality::Good);
            break;
          case 'c': // club
            cell.decoration = MapCellDecoration::SuitClub;
            break;
          case 'd': // diamond
            cell.decoration = MapCellDecoration::SuitDiamond;
            break;
          case 'h': // heart
            cell.decoration = MapCellDecoration::SuitHeart;
            break;
          case 'j': // Jesus
            cell.decoration = MapCellDecoration::Cross;
            break;
          case 's': // spade
            cell.decoration = MapCellDecoration::SuitSpade;
            break;
          case ' ': // nothing
            break;
          case '^':
          case '.':
          case '>':
          case '<':
            // action positions, nothing to do
            break;
          default:
            assert(false);
            break;
        }
      }

    }

  }

  Place generate_town(const WorldData& data, gf::Random* random)
  {
    TownMap map = generate_town_map(random);

    Place town(TownDiameter);

    for (const gf::Vec2I building_position : gf::rectangle_range(TownMapSurface)) {
      const Building& building = map(building_position);

      if (building.type == BuildingType::Empty || building.type == BuildingType::None) {
        continue;
      }

      const gf::Vec2I town_position = building_position * (TownBuildingSize  + StreetSize);
      add_building_to_town(building, town_position, town, data, random);
    }

    return town;
  }


  /*
   * LOCALITY
   */

  Place generate_locality(LocalityType type, const WorldData& data, gf::Random* random)
  {
    Place locality(LocalityDiameter);

    return locality;
  }

}
