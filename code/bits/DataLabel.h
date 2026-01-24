#ifndef FW_DATA_LABEL_H
#define FW_DATA_LABEL_H

#include <string>

#include <nlohmann/json.hpp>

#include <gf2/core/Id.h>

namespace fw {

  struct DataLabel {
    std::string tag;
    gf::Id id;

    DataLabel& operator=(std::string other) {
      tag = std::move(other);
      id = gf::hash_string(tag);
      return *this;
    }
  };

  void from_json(const nlohmann::json& json, DataLabel& label);

}

#endif // FW_DATA_LABEL_H
