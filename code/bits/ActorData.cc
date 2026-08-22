#include "ActorData.h"

#include <cassert>

namespace fw {

  NLOHMANN_JSON_SERIALIZE_ENUM( ActorType, {
    { ActorType::None, nullptr },
    { ActorType::Human, "human" },
    { ActorType::Animal, "animal" },
    { ActorType::Group, "group" },
    { ActorType::Train, "train" },
  })

  NLOHMANN_JSON_SERIALIZE_ENUM( MapCellBiome, {
    { MapCellBiome::None, nullptr },
    { MapCellBiome::Prairie, "prairie" },
    { MapCellBiome::Desert, "desert" },
    { MapCellBiome::Forest, "forest" },
    { MapCellBiome::Mountain, "mountain" },
    { MapCellBiome::Underground, "underground" },
    { MapCellBiome::Building, "building" },
  })

  void from_json(const nlohmann::json& json, ActorData& data)
  {
    json.at("label").get_to(data.label);

    ActorType type = ActorType::None;
    json.at("type").get_to(type);

    switch (type) {
      case ActorType::None:
        break;
      case ActorType::Human:
        {
          HumanElement element = {};
          json.at("display").get_to(element.display);
          json.at("body").get_to(element.body);

          data.element = element;
        }
        break;
      case ActorType::Animal:
        {
          AnimalElement element = {};
          json.at("display").get_to(element.display);
          json.at("body").get_to(element.body);
          json.at("biome").get_to(element.biome);
          json.at("can_be_mounted").get_to(element.can_be_mounted);
          json.at("can_idle").get_to(element.can_idle);

          data.element = element;
        }
        break;
      case ActorType::Group:
        break;
      case ActorType::Train:
        break;
    }

  }

}
