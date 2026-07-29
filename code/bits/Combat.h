#ifndef FW_COMBAT_H
#define FW_COMBAT_H

#include <cstdint>

#include <type_traits>

#include <nlohmann/json.hpp>

#include <gf2/core/Fixed.h>
#include <gf2/core/Random.h>

namespace fw {

  using CombatPoint = gf::Fixed<int32_t, 16>;
  using Attack = CombatPoint;
  using Defense = CombatPoint;

  static_assert(std::is_same_v<Attack, Defense>);

  struct AttackDigest {
    int8_t attribute = 0;
    int8_t luck = 0;
    int8_t modifier = 0;
    Attack attack = 0;
    int32_t range = 0;
  };

  struct DefenseDigest {
    Defense defense = 0;
  };

  std::optional<int8_t> compute_combat(const AttackDigest& attacker, const DefenseDigest& defender, gf::Random* random);

}

namespace gf {

  void from_json(const nlohmann::json& json, Fixed<int32_t, 16>& fixed);

}

#endif // FW_COMBAT_H
