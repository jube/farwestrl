#include "BodyData.h"

namespace fw {

  void from_json(const nlohmann::json& json, BodyData& data)
  {
    json.at("max_health").get_to(data.max_health);
    json.at("FOR").get_to(data.force);
    json.at("DEX").get_to(data.dexterity);
    json.at("CON").get_to(data.constitution);
    json.at("luck").get_to(data.luck);
    json.at("attack_time").get_to(data.attack_time);
    json.at("attack").get_to(data.attack);
    json.at("defense").get_to(data.defense);
  }

}
