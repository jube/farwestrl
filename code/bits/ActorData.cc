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
          HumanDataFeature feature = {};
          json.at("display").get_to(feature.display);
          json.at("body").get_to(feature.body);

          data.feature = feature;
        }
        break;
      case ActorType::Animal:
        {
          AnimalDataFeature feature = {};
          json.at("display").get_to(feature.display);
          json.at("body").get_to(feature.body);
          json.at("attack").get_to(feature.attack);
          json.at("defense").get_to(feature.defense);
          json.at("biome").get_to(feature.biome);
          json.at("can_be_mounted").get_to(feature.can_be_mounted);
          json.at("can_idle").get_to(feature.can_idle);

          data.feature = feature;
        }
        break;
      case ActorType::Group:
        break;
      case ActorType::Train:
        break;
    }

  }

}
