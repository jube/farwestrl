#include "ThingData.h"

namespace fw {

  NLOHMANN_JSON_SERIALIZE_ENUM( ThingType, {
    { ThingType::None, nullptr },
    { ThingType::Bed, "bed" },
    { ThingType::SaloonChair, "saloon_chair" },
    { ThingType::SaloonDesk, "saloon_desk" },
  })

  NLOHMANN_JSON_SERIALIZE_ENUM( ThingQuality, {
    { ThingQuality::Best, "best" },
    { ThingQuality::Good, "good" },
    { ThingQuality::Bad, "bad" },
    { ThingQuality::Worse, "worse" },
  })

  void from_json(const nlohmann::json& json, ThingData& data)
  {
    json.at("label").get_to(data.label);
    json.at("type").get_to(data.type);
    json.at("quality").get_to(data.quality);
    json.at("display").get_to(data.display);
  }

}
