#include "BodyData.h"

namespace fw {

  void from_json(const nlohmann::json& json, BodyData& data)
  {
    json.at("max_health").get_to(data.max_health);
    json.at("force").get_to(data.force);
    json.at("dexterity").get_to(data.dexterity);
    json.at("constitution").get_to(data.constitution);
    json.at("luck").get_to(data.luck);
  }

}
