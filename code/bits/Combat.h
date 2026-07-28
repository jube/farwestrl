#ifndef FW_COMBAT_H
#define FW_COMBAT_H

#include <cstdint>

#include <type_traits>

#include <nlohmann/json.hpp>

#include <gf2/core/Fixed.h>

namespace fw {

  using Attack = gf::Fixed<int32_t, 16>;
  using Defense = gf::Fixed<int32_t, 16>;

  static_assert(std::is_same_v<Attack, Defense>);

}

namespace gf {

  void from_json(const nlohmann::json& json, Fixed<int32_t, 16>& fixed);

}

#endif // FW_COMBAT_H
