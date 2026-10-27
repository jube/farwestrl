#ifndef FW_THING_DATA_H
#define FW_THING_DATA_H

#include <nlohmann/json.hpp>

#include "DataLabel.h"
#include "DisplayData.h"

namespace fw {

  struct ThingData {
    DataLabel label;
    DisplayData display;
  };

  void from_json(const nlohmann::json& j, ThingData& data);

}

#endif // FW_THING_DATA_H
