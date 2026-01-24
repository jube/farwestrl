#ifndef FW_MAP_BUILDING_H
#define FW_MAP_BUILDING_H

#include "MapState.h"

#include <array>
#include <string_view>

namespace fw {

  using TownBuildingPlan = std::array<std::u16string_view, TownBuildingSize>;

  const TownBuildingPlan& compute_town_building_plan(BuildingType building);
  char16_t compute_town_building_part(const TownBuildingPlan& building, gf::Vec2I position, gf::Direction direction);

  using LocalityBuildingPlan = std::array<std::u16string_view, LocalityDiameter>;

  const LocalityBuildingPlan& compute_locality_building_plan(LocalityType locality, uint8_t number);
  char16_t compute_locality_building_part(const LocalityBuildingPlan& building, gf::Vec2I position, gf::Direction direction);

  enum class BuildingPartType {
    None,
    Outside,
    Furniture,
    Wall,
  };

  BuildingPartType building_part_type(char16_t picture);

}

#endif // FW_MAP_BUILDING_H
