#ifndef FW_THING_DATA_H
#define FW_THING_DATA_H

#include <nlohmann/json.hpp>

#include "DataLabel.h"
#include "DisplayData.h"

namespace fw {

  enum class ThingType {
    None,
    Bed,
    SaloonChair,
    SaloonDesk,
  };

  enum class ThingQuality {
    Best,
    Good,
    Bad,
    Worse,
  };

  struct ThingData {
    DataLabel label;
    ThingType type;
    ThingQuality quality;
    DisplayData display;
  };

  void from_json(const nlohmann::json& json, ThingData& data);

}

#endif // FW_THING_DATA_H
