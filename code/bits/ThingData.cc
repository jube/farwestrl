#include "ThingData.h"

namespace fw {

  void from_json(const nlohmann::json& json, ThingData& data)
  {
    json.at("label").get_to(data.label);
    json.at("display").get_to(data.display);
  }

}
