#include "MapRuntime.h"

#include <algorithm>
#include <cstdint>

#include <gf2/core/ConsoleChar.h>
#include <gf2/core/ConsoleOperations.h>
#include <gf2/core/Direction.h>
#include <gf2/core/Easing.h>
#include <gf2/core/Math.h>

#include "Colors.h"
#include "MapCell.h"
#include "MapState.h"
#include "MapBuilding.h"
#include "NetworkState.h"
#include "Settings.h"
#include "Utils.h"
#include "WorldState.h"
#include "gf2/core/Color.h"

namespace fw {

  void FloorMap::update_minimap_explored(const std::vector<gf::Vec2I>& explored)
  {
    for (Minimap& minimap : minimaps) {
      for (const gf::Vec2I position : explored) {
        float& value = minimap.explored(position / minimap.factor);
        value += 1.0f / static_cast<float>(gf::square(minimap.factor));
        assert(value <= 1.0f);
      }
    }
  }

  namespace {

    constexpr float ColorLighterBound = 0.03f;

    char16_t generate_character(std::initializer_list<char16_t> list, gf::Random* random)
    {
      assert(list.size() > 0);
      const std::size_t index = random->compute_uniform_integer(list.size());
      assert(index < list.size());
      return std::data(list)[index];
    }

    template<typename T, T MapCell::* Field>
    uint8_t compute_neighbor_bits(const BackgroundMap& state, gf::Vec2I position, T type)
    {
      uint8_t neighbor_bits = 0b0000;
      uint8_t direction_bit = 0b0001;

      for (const gf::Orientation orientation : { gf::Orientation::North, gf::Orientation::East, gf::Orientation::South, gf::Orientation::West }) {
        const gf::Vec2I target = position + gf::displacement(orientation);

        if (!state.valid(target) || state(target).*Field == type) {
          neighbor_bits |= direction_bit;
        }

        direction_bit <<= 1;
      }

      return neighbor_bits;
    }

  }

  const FloorMap& MapRuntime::from_floor(Floor floor) const
  {
    switch (floor) {
      case Floor::Underground:
        return underground;
      case Floor::Ground:
        return ground;
      case Floor::Upstairs:
        return ground; // TODO: upstairs
    }

    assert(false);
    return ground;
  }

  FloorMap& MapRuntime::from_floor(Floor floor)
  {
    switch (floor) {
      case Floor::Underground:
        return underground;
      case Floor::Ground:
        return ground;
      case Floor::Upstairs:
        return ground; // TODO: upstairs
    }

    assert(false);
    return ground;
  }

  void MapRuntime::bind(const WorldState& state, gf::Random* random, WorldGenerationAnalysis& analysis)
  {
    analysis.set_step(WorldGenerationStep::MapGround);
    bind_ground(state, random);
    analysis.set_step(WorldGenerationStep::MapUnderground);
    bind_underground(state, random);
    analysis.set_step(WorldGenerationStep::MapRails);
    bind_railway(state);
    analysis.set_step(WorldGenerationStep::MapRoads);
    bind_roads(state, random);
    analysis.set_step(WorldGenerationStep::MapTowns);
    bind_towns(state, random);

    analysis.set_step(WorldGenerationStep::MapBuildings);
    bind_buildings(state);

    bind_reverse(state);

    analysis.set_step(WorldGenerationStep::MapMinimap);
    bind_minimaps(state);
  }

  namespace {

    std::tuple<char16_t, gf::Color> compute_decoration(const BackgroundMap& state, gf::Vec2I position, MapCellDecoration decoration, gf::Color background_color, gf::Random* random)
    {
      gf::Color foreground_color = gf::Transparent;
      char16_t character = u' ';

      switch (decoration) {
        case MapCellDecoration::None:
          break;
        case MapCellDecoration::FloorDown:
          character = u'↓';
          foreground_color = gf::darker(background_color);
          break;
        case MapCellDecoration::FloorUp:
          character = u'↑';
          foreground_color = gf::darker(background_color);
          break;
        case MapCellDecoration::Herb:
          character = generate_character({ u'.', u',', u'`', u'\'' /*, gf::ConsoleChar::SquareRoot */ }, random);
          foreground_color = gf::darker(background_color, 0.1f);
          break;
        case MapCellDecoration::Cactus:
          character = generate_character({ u'!', gf::ConsoleChar::InvertedExclamationMark }, random);
          foreground_color = gf::darker(gf::Green, 0.3f);
          break;
        case MapCellDecoration::Tree:
          character = generate_character({ u'φ', u'ψ', u'¥' }, random);
          foreground_color = gf::darker(gf::Green, 0.7f);
          break;
        case MapCellDecoration::Cliff:
          {
            const uint8_t neighbor_bits = compute_neighbor_bits<MapCellDecoration, &MapCell::decoration>(state, position, MapCellDecoration::Cliff);

            // clang-format off
            constexpr char16_t BlockCharacters[] = {
                                                    // WSEN
              gf::ConsoleChar::FullBlock,           // 0000
              gf::ConsoleChar::UpperHalfBlock,      // 0001
              gf::ConsoleChar::RightHalfBlock,      // 0010
              gf::ConsoleChar::QuadrantUpperRight,  // 0011
              gf::ConsoleChar::LowerHalfBlock,      // 0100
              gf::ConsoleChar::FullBlock,           // 0101
              gf::ConsoleChar::QuadrantLowerRight,  // 0110
              gf::ConsoleChar::FullBlock,           // 0111
              gf::ConsoleChar::LeftHalfBlock,       // 1000
              gf::ConsoleChar::QuadrantUpperLeft,   // 1001
              gf::ConsoleChar::FullBlock,           // 1010
              gf::ConsoleChar::FullBlock,           // 1011
              gf::ConsoleChar::QuadrantLowerLeft,   // 1100
              gf::ConsoleChar::FullBlock,           // 1101
              gf::ConsoleChar::FullBlock,           // 1110
              gf::ConsoleChar::FullBlock,           // 1111
            };
            // clang-format on

            assert(neighbor_bits < std::size(BlockCharacters));
            character = BlockCharacters[neighbor_bits];
            foreground_color = gf::darker(background_color, 0.5f);
          }
          break;
        case MapCellDecoration::Wall:
          character = gf::ConsoleChar::FullBlock;
          foreground_color = gf::darker(background_color, 0.5f);
          break;
        case MapCellDecoration::Rock:
          character = gf::ConsoleChar::FullBlock;
          foreground_color = RockColor;
          break;
      }

      return { character, foreground_color };
    }

    void bind_floor_map(const BackgroundMap& state, FloorMap& map, gf::Random* random)
    {
      for (const gf::Vec2I position : state.position_range()) {
        const MapCell& cell = state(position);

        gf::Color background_color = gf::White;

        switch (cell.region) {
          case MapCellBiome::None:
            background_color = gf::Transparent;
            break;
          case MapCellBiome::Prairie:
            background_color = PrairieColor;
            break;
          case MapCellBiome::Desert:
            background_color = DesertColor;
            break;
          case MapCellBiome::Forest:
            background_color = ForestColor;
            break;
          case MapCellBiome::Moutain:
            background_color = MountainColor;
            break;
          case MapCellBiome::Water:
            background_color = gf::Azure; // TODO
            break;
          case MapCellBiome::Underground:
            background_color = DirtColor;
            break;
          case MapCellBiome::Building:
            background_color = gf::Black; // TODO
            break;

        }

        background_color = gf::lighter(background_color, random->compute_uniform_float(0.0f, ColorLighterBound));
        const auto [ character, foreground_color ] = compute_decoration(state, position, cell.decoration, background_color, random);
        gf::console_write_picture(map.console, position, character, { foreground_color, background_color });

        if (!is_walkable(cell.decoration)) {
          map.background(position).properties.reset(RuntimeMapCellProperty::Walkable);
        }
      }
    }


  }

  void MapRuntime::bind_ground(const WorldState& state, gf::Random* random)
  {
    ground = FloorMap(WorldSize);
    bind_floor_map(state.map.ground, ground, random);
  }


  void MapRuntime::bind_underground(const WorldState& state, gf::Random* random)
  {
    underground = FloorMap(WorldSize);
    bind_floor_map(state.map.underground, underground, random);
  }

  namespace {

    using RailPlan = std::array<std::u16string_view, 3>;

    constexpr RailPlan RailNS = {{
      u"┼─┼",
      u"┼─┼",
      u"┼─┼",
    }};

    constexpr RailPlan RailNW = {{
      u"┼─┼",
      u"│ │",
      u"┼─╯",
    }};

    constexpr RailPlan RailNE = {{
      u"┼─┼",
      u"│ │",
      u"╰─┼",
    }};

    constexpr RailPlan RailWE = {{
      u"┼┼┼",
      u"│││",
      u"┼┼┼",
    }};

    constexpr RailPlan RailSW = {{
      u"┼─╮",
      u"│ │",
      u"┼─┼",
    }};

    constexpr RailPlan RailSE = {{
      u"╭─┼",
      u"│ │",
      u"┼─┼",
    }};

    uint8_t direction_bit(gf::Direction direction)
    {
      assert(direction != gf::Direction::Center);
      return 1 << static_cast<int8_t>(direction);
    }

    const RailPlan& compute_rail_plan(gf::Direction direction_before, gf::Direction direction_after)
    {
      const uint8_t bits = direction_bit(direction_before) | direction_bit(direction_after);

      switch (bits) {
        //     WSEN
        case 0b0011: return RailNE;
        case 0b0101: return RailNS;
        case 0b0110: return RailSE;
        case 0b1001: return RailNW;
        case 0b1010: return RailWE;
        case 0b1100: return RailSW;
        default:
          assert(false);
          break;
      }

      return RailNS;
    }

  }

  void MapRuntime::bind_railway(const WorldState& state)
  {
    gf::ConsoleStyle style;
    style.color.foreground = gf::Black;
    style.color.background = gf::Transparent;
    style.effect = gf::ConsoleEffect::none();

    const std::vector<gf::Vec2I>& railway = state.network.railway;

    // put railway on the map

    for (const auto [ index, position ] : gf::enumerate(railway)) {
      const std::size_t index_before = (index + railway.size() - 1) % railway.size();
      const gf::Vec2I position_before = railway[index_before];
      const gf::Direction direction_before = undisplacement(gf::sign(position_before - position));

      const std::size_t index_after = (index + 1) % railway.size();
      const gf::Vec2I position_after = railway[index_after];
      const gf::Direction direction_after = undisplacement(gf::sign(position_after - position));

      const RailPlan& plan = compute_rail_plan(direction_before, direction_after);

      for (int i = -1; i <= +1; ++i) {
        for (int j = -1; j <= +1; ++j) {
          const gf::Vec2I neighbor(i, j);
          const gf::Vec2I neighbor_position = position + neighbor;

          gf::console_write_picture(ground.console, neighbor_position, plan[neighbor.y + 1][neighbor.x + 1], style);
        }
      }
    }
  }

  void MapRuntime::bind_roads(const WorldState& state, gf::Random* random)
  {
    const gf::ConsoleEffect road_effect = gf::ConsoleEffect::multiply();
    const std::vector<gf::Vec2I>& roads = state.network.roads;

    for (const gf::Vec2I position : roads) {
      for (int i = -1; i <= +1; ++i) {
        for (int j = -1; j <= +1; ++j) {
          const gf::Vec2I neighbor(i, j);
          const gf::Vec2I neighbor_position = position + neighbor;

          gf::Color color = gf::lighter(gf::gray(0.9f), random->compute_uniform_float(0.0f, ColorLighterBound));
          gf::console_write_background(ground.console, neighbor_position, color, road_effect);
        }
      }
    }

  }

  void MapRuntime::bind_towns(const WorldState& state, gf::Random* random)
  {
    const gf::ConsoleEffect street_effect = gf::ConsoleEffect::alpha(0.5f);

    for (const TownState& town : state.map.towns) {
      const gf::RectI town_space = gf::RectI::from_position_size(town.position, { TownDiameter, TownDiameter });
      const gf::Vec2I town_center = town_space.center();

      for (const gf::Vec2I position : gf::rectangle_range(town_space)) {
        const float distance = gf::chebyshev_distance<float>(position, town_center);
        const float factor = (TownRadius - distance) / TownRadius;
        assert(0.0f <= factor && factor <= 1.0f);
        const float probability = 0.1f * gf::ease_out_quint(factor);

        if (random->compute_bernoulli(probability)) {
          gf::Color color = gf::lighter(StreetColor, random->compute_uniform_float(0.0f, ColorLighterBound));
          gf::console_write_background(ground.console, position, color, street_effect);
        }
      }
    }

    // streets

    for (const TownState& town : state.map.towns) {
      const int32_t horizontal_street = town.horizontal_street * (TownBuildingSize + StreetSize) - 2;
      const gf::Vec2I horizontal_position = town.position + gf::diry(horizontal_street);

      const int32_t vertical_street = town.vertical_street * (TownBuildingSize + StreetSize) - 2;
      const gf::Vec2I vertical_position = town.position + gf::dirx(vertical_street);

      static constexpr int32_t Extra = 5;

      for (int32_t i = -Extra; i < TownDiameter + Extra; ++i) {
        const gf::Vec2I position = horizontal_position + gf::dirx(i);
        gf::Color color = gf::lighter(StreetColor, random->compute_normal_float(0.0f, ColorLighterBound));
        gf::console_write_background(ground.console, position, color, street_effect);

        if (position.x != vertical_position.x) {
          color = gf::lighter(StreetColor, random->compute_normal_float(0.0f, ColorLighterBound));
          gf::console_write_background(ground.console, position + gf::diry(-1), color, street_effect);
          color = gf::lighter(StreetColor, random->compute_normal_float(0.0f, ColorLighterBound));
          gf::console_write_background(ground.console, position + gf::diry(+1), color, street_effect);
        }
      }


      for (int32_t i = -Extra; i < TownDiameter + Extra; ++i) {
        const gf::Vec2I position = vertical_position + gf::diry(i);
        gf::Color color = gf::lighter(StreetColor, random->compute_normal_float(0.0f, ColorLighterBound));
        gf::console_write_background(ground.console, position, color, street_effect);

        if (position.y != horizontal_position.y) {
          color = gf::lighter(StreetColor, random->compute_normal_float(0.0f, ColorLighterBound));
          gf::console_write_background(ground.console, position + gf::dirx(-1), color, street_effect);
          color = gf::lighter(StreetColor, random->compute_normal_float(0.0f, ColorLighterBound));
          gf::console_write_background(ground.console, position + gf::dirx(+1), color, street_effect);
        }
      }
   }
  }

  // void MapRuntime::blur(const WorldState& state)
  // {
  //   gf::Console new_ground = ground.console;
  //
  //   constexpr int KernelSize = 3;
  //   constexpr int KernelCenter = (KernelSize - 1) / 2;
  //   constexpr int Kernel[KernelSize][KernelSize] = {
  //     {  1,  2,  1 },
  //     {  2, 16,  2 },
  //     {  1,  2,  1 }
  //   };
  //
  //   for (gf::Vec2I position : state.map.cells.position_range()) {
  //     gf::Color new_color = Kernel[KernelCenter][KernelCenter] * ground.console.background(position);
  //     int total_weight = Kernel[KernelCenter][KernelCenter];
  //
  //     for (const gf::Vec2I neighbor : state.map.cells.compute_8_neighbors_range(position)) {
  //       const gf::Vec2I offset = KernelCenter + (position - neighbor);
  //       assert(0 <= offset.x && offset.x < KernelSize);
  //       assert(0 <= offset.y && offset.y < KernelSize);
  //       const int weight = Kernel[offset.x][offset.y];
  //       new_color += weight * ground.console.background(neighbor);
  //       total_weight += weight;
  //     }
  //
  //     new_color /= total_weight;
  //     new_ground.set_background(position, new_color);
  //   }
  //
  //   ground.console = std::move(new_ground);
  // }

  namespace {

    gf::ConsoleColorStyle building_style(BuildingPartType type)
    {
      const gf::Color base_color = 0xcb9651;
      const gf::Color decoration_color = gf::darker(base_color, 0.25f);
      const gf::Color furniture_color = gf::darker(base_color);
      const gf::Color wall_color = gf::darker(furniture_color);

      switch (type) {
        case BuildingPartType::None:
          return { decoration_color, base_color };
        case BuildingPartType::Outside:
          return { gf::Transparent, gf::Transparent };
        case BuildingPartType::Furniture:
          return { furniture_color, base_color };
        case BuildingPartType::Wall:
          return { base_color, wall_color  }; // inverted
      }

      assert(false);
      return { gf::darker(gf::Red), gf::Red };
    }

  }

  void MapRuntime::bind_buildings(const WorldState& state)
  {
    // buildings

    for (const TownState& town : state.map.towns) {
      for (int32_t i = 0; i < TownsBlockSize; ++i) {
        for (int32_t j = 0; j < TownsBlockSize; ++j) {
          const gf::Vec2I block_position = { i, j };
          const Building& building = town(block_position);

          if (building.type == BuildingType::Empty || building.type == BuildingType::None) {
            continue;
          }

          const TownBuildingPlan& plan = compute_town_building_plan(building.type);

          for (int32_t y = 0; y < TownBuildingSize; ++y) {
            for (int32_t x = 0; x < TownBuildingSize; ++x) {
              const gf::Vec2I position = { x, y };
              const char16_t part = compute_town_building_part(plan, position, building.direction);
              const BuildingPartType type = building_part_type(part);

              const gf::Vec2I map_position = town.position + block_position * (TownBuildingSize + StreetSize) + position;

              gf::ConsoleStyle style;
              style.color = building_style(type);
              style.effect = gf::ConsoleEffect::set();

              gf::console_write_picture(ground.console, map_position, part, style);

              switch (type) {
                case BuildingPartType::None:
                case BuildingPartType::Outside:
                  // nothing to do
                  break;
                case BuildingPartType::Furniture:
                case BuildingPartType::Wall:
                  ground.background(map_position).properties.reset(RuntimeMapCellProperty::Walkable);
                  break;
              }
            }
          }
        }
      }
    }

    for (const LocalityState& locality : state.map.localities) {
      const LocalityBuildingPlan& plan = compute_locality_building_plan(locality.type, locality.number);
      const gf::Vec2I base_position = locality.position - LocalityRadius;

      for (int32_t y = 0; y < LocalityDiameter; ++y) {
        for (int32_t x = 0; x < LocalityDiameter; ++x) {
          const gf::Vec2I position = { x, y };
          const char16_t part = compute_locality_building_part(plan, position, locality.direction);
          const BuildingPartType type = building_part_type(part);

          const gf::Vec2I map_position = base_position + position;

          if (type != BuildingPartType::Outside) {
            gf::ConsoleStyle style;
            style.color = building_style(type);
            style.effect = gf::ConsoleEffect::set();

            gf::console_write_picture(ground.console, map_position, part, style);
          } else {
            assert(part == u'.');
          }

          switch (type) {
            case BuildingPartType::None:
            case BuildingPartType::Outside:
              // nothing to do
              break;
            case BuildingPartType::Furniture:
            case BuildingPartType::Wall:
              ground.background(map_position).properties.reset(RuntimeMapCellProperty::Walkable);
              break;
          }
        }
      }
    }

  }

  void MapRuntime::bind_reverse(const WorldState& state)
  {
    for (const auto& [ index, actor ] : gf::enumerate(state.actors)) {
      FloorMap& floor = from_floor(actor.floor);
      floor.reverse(actor.position).actor_index = uint32_t(index);
    }
  }

  namespace {

    char16_t compute_minimap_rail_plan(gf::Direction direction_before, gf::Direction direction_after)
    {
      const uint8_t bits = direction_bit(direction_before) | direction_bit(direction_after);

      // if both directions are equal, that means the railway made a U-turn that
      // is not viewable on the minimap

      switch (bits) {
        //     WSEN
        case 0b0001: return u'│';
        case 0b0010: return u'─';
        case 0b0011: return u'└';
        case 0b0100: return u'│';
        case 0b0101: return u'│';
        case 0b0110: return u'┌';
        case 0b1000: return u'─';
        case 0b1001: return u'┘';
        case 0b1010: return u'─';
        case 0b1100: return u'┐';
        default:
          gf::Log::debug("bits: {:b}", bits);
          assert(false);
          break;
      }

      return u'│';
    }

  }

  namespace {

    Minimap compute_base_minimap(const BackgroundMap& state, int factor) {
      /*
       * console
       */

      gf::Console console(WorldSize / factor);

      // base colors

      for (const gf::Vec2I position : gf::position_range(console.size())) {
        gf::Color color = gf::Transparent;

        std::array<int, MapCellBiomeCount> count = { };

        for (const gf::Vec2I offset : gf::position_range({ factor, factor })) {
          gf::Vec2I origin_position = position * factor + offset;
          const MapCellBiome region = state(origin_position).region;
          const std::size_t index = static_cast<std::size_t>(region);
          assert(index < count.size());
          ++count[index];
        }

        const auto iterator = std::max_element(std::begin(count), std::end(count));
        const std::ptrdiff_t index = iterator - std::begin(count);
        assert(0 <= index && std::size_t(index) < count.size());
        const MapCellBiome region = static_cast<MapCellBiome>(index);

        switch (region) {
          case MapCellBiome::None:
            color = gf::Transparent;
            break;
          case MapCellBiome::Prairie:
            color = PrairieColor;
            break;
          case MapCellBiome::Desert:
            color = DesertColor;
            break;
          case MapCellBiome::Forest:
            color = ForestColor;
            break;
          case MapCellBiome::Moutain:
            color = MountainColor;
            break;
          case MapCellBiome::Water:
            color = gf::Azure; // TODO
            break;
          case MapCellBiome::Underground:
            color = DirtColor;
            break;
          case MapCellBiome::Building:
            color = StreetColor; // TODO
            break;
        }

        gf::console_write_background(console, position, color);
      }

      /*
       * explored
       */

      gf::Array2D<float> explored(WorldSize / factor);

      for (const gf::Vec2I position : gf::position_range(explored.size())) {
        int count = 0;

        for (const gf::Vec2I offset : gf::position_range({ factor, factor })) {
          gf::Vec2I origin_position = position * factor + offset;

          if (state(origin_position).explored()) {
            ++count;
          }
        }

        explored(position) = static_cast<float>(count) / static_cast<float>(gf::square(factor));
      }

      return { console, explored, factor };
    }

    Minimap compute_ground_minimap(const WorldState& state, int factor) {
      Minimap minimap = compute_base_minimap(state.map.ground, factor);

      // towns

      // for (const TownState& town : state.map.towns) {
      //   const gf::RectI town_space = gf::RectI::from_position_size(town.position, { TownDiameter, TownDiameter });
      //
      //   for (const gf::Vec2I position : gf::rectangle_range(town_space)) {
      //     minimap.set_background(position / factor, StreetColor);
      //   }
      // }

      // train

      const std::vector<gf::Vec2I>& railway = state.network.railway;
      std::vector<gf::Vec2I> minimap_railway;

      for (const gf::Vec2I position : railway) {
        const gf::Vec2I minimap_position = position / factor;

        if (minimap_railway.empty() || minimap_position != minimap_railway.back()) {
          assert(minimap_railway.empty() || gf::manhattan_distance(minimap_railway.back(), minimap_position) == 1);
          minimap_railway.push_back(minimap_position);
        }
      }

      if (minimap_railway.front() == minimap_railway.back()) {
        minimap_railway.pop_back();
      }

      for (const auto [ index, position ] : gf::enumerate(minimap_railway)) {
        const std::size_t index_before = (index + minimap_railway.size() - 1) % minimap_railway.size();
        const gf::Vec2I position_before = minimap_railway[index_before];
        const gf::Direction direction_before = undisplacement(gf::sign(position_before - position));

        const std::size_t index_after = (index + 1) % minimap_railway.size();
        const gf::Vec2I position_after = minimap_railway[index_after];
        const gf::Direction direction_after = undisplacement(gf::sign(position_after - position));

        const char16_t picture = compute_minimap_rail_plan(direction_before, direction_after);

        gf::console_write_picture(minimap.console, position, picture, gf::Black);
      }

      // roads

      std::vector<gf::Vec2I> minimap_roads;

      for (const gf::Vec2I position : state.network.roads) {
        minimap_roads.push_back(position / factor);
      }

      std::sort(minimap_roads.begin(), minimap_roads.end(), [](gf::Vec2I lhs, gf::Vec2I rhs) {
        return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
      });

      minimap_roads.erase(std::unique(minimap_roads.begin(), minimap_roads.end()), minimap_roads.end());

      for (const gf::Vec2I position : minimap_roads) {
        const gf::Color background = minimap.console(position).parts[0].background; // TODO
        gf::console_write_background(minimap.console, position, gf::darker(background, 0.2f / factor));
      }

      return minimap;
    }

    Minimap compute_underground_minimap(const WorldState& state, int factor) {
      return compute_base_minimap(state.map.underground, factor);
    }

  }

  void MapRuntime::bind_minimaps(const WorldState& state)
  {
    ground.minimaps[0] = compute_ground_minimap(state, 4);
    ground.minimaps[1] = compute_ground_minimap(state, 8);
    ground.minimaps[2] = compute_ground_minimap(state, 16);
    ground.minimaps[3] = compute_ground_minimap(state, 32);

    underground.minimaps[0] = compute_underground_minimap(state, 2);
    underground.minimaps[1] = compute_underground_minimap(state, 4);
    underground.minimaps[2] = compute_underground_minimap(state, 8);
    underground.minimaps[3] = compute_underground_minimap(state, 16);
  }

}
