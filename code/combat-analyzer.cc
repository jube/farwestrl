#include <print>

#include "bits/Combat.h"

#include <gf2/core/Random.h>

constexpr int LoCount = 20;
constexpr int HiCount = 100000;

int main()
{
  const fw::AttackDigest attacker = {
    .attribute = 13,
    .luck = 10,
    .modifier = 0,
    .attack = 20,
    .range = 0,
  };

  const fw::DefenseDigest defender = {
    .defense = 5,
  };

  gf::Random random;

  for (int i = 0; i < LoCount; ++i) {
    std::optional<int8_t> damage = compute_combat(attacker, defender, &random);

    if (damage) {
      std::println("Attack succeeded! Damage: {}", damage.value());
    } else {
      std::println("Attack failed!");
    }
  }

  int failures = 0;
  int avg_damage = 0;

  for (int i = 0; i < HiCount; ++i) {
    std::optional<int8_t> damage = compute_combat(attacker, defender, &random);

    if (damage) {
      avg_damage += damage.value();
    } else {
      ++failures;
    }
  }

  const double theoretical_failure = (20.0 - (attacker.attribute + attacker.modifier)) / 20.0 * (20.0 - attacker.luck) / 20.0 * 100.0;

  if (failures == HiCount) {
    std::println("100%% failures.");
  } else {
    std::println("{:g}% failures ({:g}%). {:g} average damage. {:g} global average damage.", static_cast<double>(failures) / static_cast<double>(HiCount) * 100.0, theoretical_failure, static_cast<double>(avg_damage) / static_cast<double>(HiCount - failures), static_cast<double>(avg_damage) / static_cast<double>(HiCount));
  }


}
