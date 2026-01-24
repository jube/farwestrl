#include "MapBuilding.h"

#include <algorithm>

#include "Pictures.h"

namespace fw {

  template<int32_t Size, typename Plan>
  char16_t compute_generic_building_part(const Plan& building, gf::Vec2I position, gf::Direction direction)
  {
    assert(0 <= position.x && position.x < Size);
    assert(0 <= position.y && position.y < Size);

    char16_t picture = u'#';

    switch (direction) {
      case gf::Direction::Up:
        picture = building[position.y][position.x];
        break;
      case gf::Direction::Right:
        picture = building[Size - position.x - 1][position.y];
        break;
      case gf::Direction::Down:
        picture = building[Size - position.y - 1][Size - position.x - 1];
        break;
      case gf::Direction::Left:
        picture = building[position.x][Size - position.y - 1];
        break;
      default:
        assert(false);
        break;
    }

    return rotate_picture(picture, direction);
  }

  constexpr TownBuildingPlan Bank = {{
    u"╔═════════╗",
    u"║         ║",
    u"║         ║",
    u"╠════─════╣",
    u"║         ║",
    u"║ ███████ ║",
    u"║   $·$   ║",
    u"║   $ $   ║",
    u"║   $ $   ║",
    u"║   $ $   ║",
    u"╚════─════╝",
  }};

  constexpr TownBuildingPlan Casino = {{
    u"╔═════════╗",
    u"║       ·≡║",
    u"║ █· ♥  ·≡║",
    u"║ █·    ·≡║",
    u"║ █· ♣  ·≡║",
    u"║       ·≡║",
    u"║ █· ♦  ·≡║",
    u"║ █·    ·≡║",
    u"║ █· ♠  ·≡║",
    u"║       ·≡║",
    u"╚════─════╝",
  }};

  constexpr TownBuildingPlan Church = {{
    u"╔═─═══════╗",
    u"║         ║",
    u"║ ┼     ┼ ║",
    u"║ │ ███ │ ║",
    u"║         ║",
    u"║ └─┘ └─┘ ║",
    u"║ └─┘ └─┘ ║",
    u"║ └─┘ └─┘ ║",
    u"║ └─┘ └─┘ ║",
    u"║ └─┘ └─┘ ║",
    u"╚════─════╝",
  }};

  constexpr TownBuildingPlan ClothShop = {{
    u"╔════╦════╗",
    u"║=··=║=··=║",
    u"║=··=║=··=║",
    u"║=··=║=··=║",
    u"║=·     ·=║",
    u"║=·     ·=║",
    u"║=· ███ ·=║",
    u"║=·  ·  ·=║",
    u"║=·     ·=║",
    u"║=·     ·=║",
    u"╚════─════╝",
  }};

  constexpr TownBuildingPlan FoodShop = {{
    u"╔═════════╗",
    u"║▒  === ·=║",
    u"╠═  ··· ·=║",
    u"║=· ··· ·=║",
    u"║=· === ·=║",
    u"║=· ··· ·=║",
    u"║       ·=║",
    u"║  ·    ·=║",
    u"║ ███   ·=║",
    u"║       ·=║",
    u"╚════─════╝",
  }};

  constexpr TownBuildingPlan Hotel = {{
    u"╔═══╦═╦═══╗",
    u"║   ║▒║   ║",
    u"║   ║ ║   ║",
    u"║   ║ │   ║",
    u"║   │ ║   ║",
    u"╠═══╣ ╚═══╣",
    u"║   ║     ║",
    u"║   │   █ ║",
    u"║   ║  ·█ ║",
    u"║   ║   █ ║",
    u"╚═══╩─══╧═╝",
  }};

  constexpr TownBuildingPlan House1 = {{
    u"╔════╦═╦══╗",
    u"║    ║ ║  ║",
    u"║    │ ║  ║",
    u"╠════╣ │  ║",
    u"║    │ ║  ║",
    u"║    ║ ║  ║",
    u"╠════╝ ╚══╣",
    u"║         ║",
    u"║ ██      ║",
    u"║         ║",
    u"╚════─════╝",
  }};

  constexpr TownBuildingPlan House2 = {{
    u"╔═══╦═════╗",
    u"║   ║     ║",
    u"║   ║     ║",
    u"║   ║     ║",
    u"╠═─═╩─╦═══╣",
    u"║     ║   ║",
    u"║     │   ║",
    u"║ █   ║   ║",
    u"║ █   ║   ║",
    u"║   ║ ║   ║",
    u"╚═══╩─╩═══╝",
  }};

  constexpr TownBuildingPlan House3 = {{
    u"╔═════════╗",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"╚════─════╝",
  }};

  constexpr TownBuildingPlan MarshalOffice = {{
    u"╔══─══════╗",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"╠══─═══╦══╣",
    u"║      ·  ║",
    u"║   ██ │  ║",
    u"║ █  · ╠══╣",
    u"║ █·   ·  ║",
    u"║      │  ║",
    u"╚════─═╩══╝",
  }};

  constexpr TownBuildingPlan Restaurant = {{
    u"╔═════════╗",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"╚════─════╝",
  }};

  constexpr TownBuildingPlan Saloon = {{
    u"╔═══════╦═╗",
    u"║       ║▒║",
    u"╟██████ ║ ║",
    u"║  · ·    ║",
    u"║ ·     · ║",
    u"║·•·   ·•·║",
    u"║ ·     · ║",
    u"║  ·   ·  ║",
    u"║ ·•· ·•· ║",
    u"║  ·   ·  ║",
    u"╚════─════╝",
  }};

  constexpr TownBuildingPlan School = {{
    u"╔═─═══════╗",
    u"║         ║",
    u"║   ███   ║",
    u"║         ║",
    u"║  └┘ └┘  ║",
    u"║  └┘ └┘  ║",
    u"║  └┘ └┘  ║",
    u"║  └┘ └┘  ║",
    u"║  └┘ └┘  ║",
    u"║  └┘ └┘  ║",
    u"╚════─════╝",
  }};

  constexpr TownBuildingPlan WeaponShop = {{
    u"╔═════════╗",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"╚════─════╝",
  }};


  constexpr TownBuildingPlan Template = {{
    u"╔═════════╗",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"║         ║",
    u"╚════─════╝",
  }};

  const TownBuildingPlan& compute_town_building_plan(BuildingType building)
  {
    switch (building) {
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
        return Restaurant;
      case BuildingType::Saloon:
        return Saloon;
      case BuildingType::School:
        return School;
      case BuildingType::WeaponShop:
        return WeaponShop;
      default:
        assert(false);
        break;
    }

    return Template;
  }


  char16_t compute_town_building_part(const TownBuildingPlan& building, gf::Vec2I position, gf::Direction direction)
  {
    return compute_generic_building_part<TownBuildingSize>(building, position, direction);
  }


  using LocalityBuildingPlan = std::array<std::u16string_view, LocalityDiameter>;;

  constexpr LocalityBuildingPlan Farm = {{
    u".......┌──────────────────┐",
    u".......│..................│",
    u".......│..................│",
    u".......│..................│",
    u".......│..................│",
    u".......│..................│",
    u".......│..................│",
    u".......│..................│",
    u".......│..................│",
    u".......│..................│",
    u".......│..................│",
    u".......│..................│",
    u".......│..................│",
    u".......└──────────────────┘",
    u"...........................",
    u"...........................",
    u"╔═════════╗................",
    u"║         ║................",
    u"║         ║................",
    u"║  █████  │................",
    u"║         ║................",
    u"║         ║................",
    u"╠═══─═════╣................",
    u"║         ║................",
    u"║         ║................",
    u"║         ║................",
    u"╚═════════╝................",
  }};

  constexpr LocalityBuildingPlan Village = {{
    u"...........................",
    u"............╔═══╗..........",
    u"...........╔╝   ╚╗.........",
    u"..........╔╝     ╚╗........",
    u"..........║       ║........",
    u"..........║       ║........",
    u"..........║       ║........",
    u"..╔═╗.....╚╗     ╔╝........",
    u".╔╝ ╚╗.....╚╗   ╔╝.........",
    u"╔╝   ╚╗.....╚═─═╝..........",
    u"║     │...........╔═╗......",
    u"╚╗   ╔╝..........╔╝ ╚╗.....",
    u".╚╗ ╔╝..........╔╝   ╚╗....",
    u"..╚═╝...........│     ║....",
    u"................╚╗   ╔╝....",
    u"..╔═╗............╚╗ ╔╝.....",
    u".╔╝ ╚╗............╚═╝......",
    u"╔╝   ╚╗....................",
    u"║     │..............╔═╗...",
    u"╚╗   ╔╝.............╔╝ ╚╗..",
    u".╚╗ ╔╝....╔─╗......╔╝   ╚╗.",
    u"..╚═╝....╔╝ ╚╗.....│     ║.",
    u"........╔╝   ╚╗....╚╗   ╔╝.",
    u"........║     ║.....╚╗ ╔╝..",
    u"........╚╗   ╔╝......╚═╝...",
    u".........╚╗ ╔╝.............",
    u"..........╚═╝..............",
  }};

  constexpr LocalityBuildingPlan Camp = {{
    u"╔═══╗.................╔═══╗",
    u"║   ╠═══════───═╦═════╣   ║",
    u"║   │...........║     ║   ║",
    u"║   ║...........║ ·█· ║   ║",
    u"╚╦══╝...........║ ·█· ╚─═╦╝",
    u".║..............║ ·█·    ║.",
    u".║.╔═══─═══╗....│ ·█·    ║.",
    u".║.║       ║....║ ·█·    ║.",
    u".║.║       ║....║ ·█·    ║.",
    u".║.║       ║....║        ║.",
    u".║.║       ║....╚════════╣.",
    u".║.║       ║.............║.",
    u".║.║       ║...┌───────┐.║.",
    u".║.║       ║...│.......│.║.",
    u".║.║       ║...│.......│.║.",
    u".║.║       ║...│.......│.║.",
    u".║.║       ║...│.......│.║.",
    u".║.║       ║...│.......│.║.",
    u".║.║       ║...│.......│.║.",
    u".║.║       ║...│.......│.║.",
    u".║.╚═══─═══╝...└───────┘.║.",
    u".║.......................║.",
    u"╔╩══╗.................╔══╩╗",
    u"║   ║.................║   ║",
    u"║   │.................│   ║",
    u"║   ╠═══════───═══════╣   ║",
    u"╚═══╝.................╚═══╝",
  }};

  const LocalityBuildingPlan& compute_locality_building_plan(LocalityType locality, [[maybe_unused]] uint8_t number)
  {
    assert(number == 0); // TODO

    switch (locality) {
      case LocalityType::Farm:
        return Farm;
      case LocalityType::Camp:
        return Camp;
      case LocalityType::Village:
        return Village;
    }

    return Farm;
  }

  char16_t compute_locality_building_part(const LocalityBuildingPlan& building, gf::Vec2I position, gf::Direction direction)
  {
    return compute_generic_building_part<LocalityDiameter>(building, position, direction);
  }

  BuildingPartType building_part_type(char16_t picture)
  {
    if (picture == u'.') {
      return BuildingPartType::Outside;
    }

    constexpr std::u16string_view Walls = u"║═╣╩╠╦╚╔╗╝╢╧╟╤╡╨╞╥";

    if (std::find(Walls.begin(), Walls.end(), picture) != Walls.end()) {
      return BuildingPartType::Wall;
    }

    constexpr std::u16string_view Furnitures = u"█•=≡";

    if (std::find(Furnitures.begin(), Furnitures.end(), picture) != Furnitures.end()) {
      return BuildingPartType::Furniture;
    }

    return BuildingPartType::None;
  }

}
