#include "Combat.h"

namespace gf {

  void from_json(const nlohmann::json& json, Fixed<int32_t, 16>& fixed)
  {
    float value = 0.0f;
    json.get_to(value);
    fixed = value;
  }

}
