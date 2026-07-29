#include "Combat.h"

#include <gf2/core/Dice.h>

namespace fw {

  std::optional<int8_t> compute_combat(const AttackDigest& attacker, const DefenseDigest& defender, gf::Random* random)
  {
    /*
     * Step 1. Evaluate the success of the attack
     */

    gf::Dice dice(20, 1);

    const int8_t modified_attribute = attacker.attribute + attacker.modifier;
    const int8_t attribute_test = dice.roll(random);
    Attack strength = 0;

    if (attribute_test > modified_attribute) {
      // failed the attribute test

      const int8_t luck_test = dice.roll(random);

      if (luck_test > attacker.luck) {
        // failed the luck test
        return std::nullopt;
      }

      strength = -0.2f;
    } else {
      strength = static_cast<float>(modified_attribute - attribute_test) / 20.0f;
    }

    /*
     * Step 2. Compute the damage points
     */

    const CombatPoint damage = attacker.attack * (1 + strength) - defender.defense;

    if (damage <= 0) {
      return 0;
    }

    return damage.ceil();
  }

}

namespace gf {

  void from_json(const nlohmann::json& json, Fixed<int32_t, 16>& fixed)
  {
    float value = 0.0f;
    json.get_to(value);
    fixed = value;
  }

}
