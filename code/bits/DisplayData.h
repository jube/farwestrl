#ifndef FW_DISPLAY_DATA_H
#define FW_DISPLAY_DATA_H

#include <nlohmann/json.hpp>

#include <gf2/core/Color.h>

namespace fw {

  struct DisplayData {
    gf::Color color;
    char16_t picture;
  };

  void from_json(const nlohmann::json& json, DisplayData& data);

}


#endif // FW_DISPLAY_DATA_H
