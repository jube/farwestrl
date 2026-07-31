#include <map>
#include <print>
#include <ranges>
#include <stdexcept>

#include <gf2/core/Dice.h>
#include <gf2/core/Random.h>

#include "bits/ActorData.h"
#include "bits/Combat.h"
#include "bits/ItemData.h"
#include "bits/Times.h"
#include "bits/WorldData.h"

#include "config.h"

constexpr int LoCount = 20;
constexpr int HiCount = 100000;

constexpr double MinLevel = 3.0;
constexpr double MaxLevel = 100.0;

constexpr std::size_t Iterations = 2000;
constexpr std::size_t Neighbors = 20;

int dummy()
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
    std::optional<int8_t> damage = fw::compute_combat(attacker, defender, &random);

    if (damage) {
      std::println("Attack succeeded! Damage: {}", damage.value());
    } else {
      std::println("Attack failed!");
    }
  }

  int failures = 0;
  int avg_damage = 0;

  for (int i = 0; i < HiCount; ++i) {
    std::optional<int8_t> damage = fw::compute_combat(attacker, defender, &random);

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

  return 0;
}

namespace {

  template<typename T>
  T find_or_throw(const std::map<std::string, T, std::less<>>& map, std::string_view name, const char* what)
  {
    if (auto iterator = map.find(name); iterator != map.end()) {
      return iterator->second;
    }

    throw std::runtime_error(what);
  }

  struct Weapons {
    std::map<std::string, fw::MeleeWeaponDataFeature, std::less<>> melee_weapons;
    std::map<std::string, fw::DistanceWeaponDataFeature, std::less<>> distance_weapons;
    std::map<std::string, fw::ProjectileDataFeature, std::less<>> projectiles;

    fw::MeleeWeaponDataFeature melee_weapon(std::string_view name) const
    {
      return find_or_throw(melee_weapons, name, "Unknown melee weapon");
    }
  };

  Weapons compute_weapons(const fw::WorldData& data)
  {
    Weapons weapons;

    for (const fw::ItemData& item : data.items) {
      switch (item.type()) {
        case fw::ItemType::None:
          break;
        case fw::ItemType::MeleeWeapon:
          weapons.melee_weapons.emplace(item.label.tag, item.feature.from<fw::ItemType::MeleeWeapon>());
          break;
        case fw::ItemType::DistanceWeapon:
          weapons.distance_weapons.emplace(item.label.tag, item.feature.from<fw::ItemType::DistanceWeapon>());
          break;
        case fw::ItemType::Projectile:
          weapons.projectiles.emplace(item.label.tag, item.feature.from<fw::ItemType::Projectile>());
          break;
      }
    }

    return weapons;
  }

  struct Actors {
    std::map<std::string, fw::HumanDataFeature, std::less<>> humans;
    std::map<std::string, fw::AnimalDataFeature, std::less<>> animals;

    fw::HumanDataFeature human(std::string_view name) const
    {
      return find_or_throw(humans, name, "Unknown human");
    }

    fw::AnimalDataFeature animal(std::string_view name) const
    {
      return find_or_throw(animals, name, "Unknown animal");
    }
  };

  Actors compute_actors(const fw::WorldData& data)
  {
    Actors actors;

    for (const fw::ActorData& actor : data.actors) {
      switch (actor.type()) {
        case fw::ActorType::None:
        case fw::ActorType::Group:
        case fw::ActorType::Train:
          break;
        case fw::ActorType::Human:
          actors.humans.emplace(actor.label.tag, actor.feature.from<fw::ActorType::Human>());
          break;
        case fw::ActorType::Animal:
          actors.animals.emplace(actor.label.tag, actor.feature.from<fw::ActorType::Animal>());
          break;
      }
    }

    return actors;
  }

  struct CombatDigest {
    // std::string name;
    double health;
    double attribute;
    double luck;
    double range;
    double modifier;
    double use_time;
  };

  double compute_average_from_dice(std::string_view spec)
  {
    gf::Dice dice(spec);
    return (static_cast<double>(dice.faces()) + 1) / 2.0 * dice.count() + dice.modifier();
  }

  CombatDigest compute_combat_digest(const fw::AnimalDataFeature& data)
  {
    return {
      .health = static_cast<double>(data.body.max_health),
      .attribute = compute_average_from_dice(data.body.force),
      .luck = compute_average_from_dice(data.body.luck),
      .range = 0.0,
      .modifier = 0.0,
      .use_time = static_cast<double>(data.body.attack_time),
    };
  }

  CombatDigest compute_combat_digest(const fw::HumanDataFeature& data, const fw::MeleeWeaponDataFeature& weapon)
  {
    return {
      .health = static_cast<double>(data.body.max_health),
      .attribute = compute_average_from_dice(data.body.force),
      .luck = compute_average_from_dice(data.body.luck),
      .range = 0.0,
      .modifier = static_cast<double>(weapon.modifier),
      .use_time = static_cast<double>(weapon.use_time),
    };
  }

  struct CombatLevel {
    double attack;
    double defense;
  };

  double evaluate_average_damage_points(const CombatDigest& digest, const CombatLevel& attacker, const CombatLevel& defender)
  {
    const double first_roll_success = (digest.attribute + digest.modifier) / 20.0;
    const double first_roll_damage = first_roll_success * std::max(first_roll_success / 2.0 * attacker.attack - defender.defense, 0.0);
    const double second_roll_success = (1.0 - first_roll_success) * digest.luck;
    const double second_roll_damage = second_roll_success * std::max(0.8 * attacker.attack - defender.defense, 0.0);
    return std::max(first_roll_damage + second_roll_damage, std::numeric_limits<double>::epsilon());
  }

  double evaluate_time_to_beat(const CombatDigest& attacker, const CombatDigest& defender, double damage_points)
  {
    const double walk_time = static_cast<double>(fw::StraightWalkTime) * std::max(defender.range - attacker.range, 0.0);
    const double attack_time = defender.health / damage_points;
    return walk_time + attack_time;
  }

  struct CombatEvaluation {
    std::array<double, 2> times;
  };

  CombatEvaluation evalutate_combat(const CombatDigest& lhs_digest, const CombatLevel& lhs_level, const CombatDigest& rhs_digest, const CombatLevel& rhs_level)
  {
    CombatEvaluation evaluation = {};

    const double lhs_damage_points = evaluate_average_damage_points(lhs_digest, lhs_level, rhs_level);
    evaluation.times[0] = evaluate_time_to_beat(lhs_digest, rhs_digest, lhs_damage_points);

    const double rhs_damage_points = evaluate_average_damage_points(rhs_digest, rhs_level, lhs_level);
    evaluation.times[1] = evaluate_time_to_beat(rhs_digest, lhs_digest, rhs_damage_points);

    return evaluation;
  }

  double evaluate(const std::vector<CombatDigest>& digests, const std::vector<CombatLevel>& levels)
  {
    assert(digests.size() == levels.size());
    double value = 0.0;

    for (const auto & [ i, lhs_digest ] : std::views::enumerate(digests)) {
      const CombatLevel& lhs_level = levels[i];

      for (std::size_t j = i + 1; j < digests.size(); ++j) {
        const CombatDigest& rhs_digest = digests[j];
        const CombatLevel& rhs_level = levels[j];

        const CombatEvaluation evaluation = evalutate_combat(lhs_digest, lhs_level, rhs_digest, rhs_level);

        value += (j - i) * std::max(0.0, evaluation.times[1] - evaluation.times[0]);

        value += (j - i) * std::max(0.0, lhs_level.attack - rhs_level.attack);
        value += (j - i) * std::max(0.0, lhs_level.defense - rhs_level.defense);

        value += (lhs_level.attack + rhs_level.attack + lhs_level.defense + rhs_level.defense) / 4.0;

      }

    }

    return value;
  }

  std::vector<CombatLevel> compute_random_values(std::size_t size, gf::Random* random)
  {
    std::vector<CombatLevel> levels(size);

    for (CombatLevel& level : levels) {
      level.attack = random->compute_uniform_float(MinLevel, MaxLevel);
      level.defense = random->compute_uniform_float(MinLevel, MaxLevel);
    }

    return levels;
  }

  std::vector<CombatLevel> compute_neighbor(const std::vector<CombatLevel>& levels, gf::Random* random)
  {
    const std::size_t index = random->compute_uniform_integer(levels.size());
    assert(index < levels.size());

    std::vector<CombatLevel> copy = levels;

    copy[index].attack = random->compute_uniform_float(MinLevel, MaxLevel);
    copy[index].defense = random->compute_uniform_float(MinLevel, MaxLevel);
    return copy;
  }

  void dummy2()
  {
    const std::filesystem::path data_directory = fw::FarWestDataDirectory;

    gf::Random random;

    fw::WorldData data;
    data.load_from_file(data_directory / "data.json");

    Weapons weapons = compute_weapons(data);
    Actors actors = compute_actors(data);

    const std::vector<CombatDigest> opponents = {
      compute_combat_digest(actors.animal("Scorpion")),
      compute_combat_digest(actors.animal("Snake")),
      compute_combat_digest(actors.animal("Cow")),
      compute_combat_digest(actors.animal("Horse")),
      compute_combat_digest(actors.animal("Coyote")),
      compute_combat_digest(actors.human("Hero"), weapons.melee_weapon("Knife")),
      compute_combat_digest(actors.animal("Grizzli")),
      compute_combat_digest(actors.animal("Bison")),
    };

    const std::size_t count = opponents.size();

    std::vector<CombatLevel> current = compute_random_values(count, &random);
    double current_value = evaluate(opponents, current);

    std::vector<CombatLevel> best = current;
    double best_value = current_value;

    for (std::size_t i = 0; i < Iterations; ++i) {
      std::vector<CombatLevel> candidate;
      double candidate_value;

      for (std::size_t j = 0; j < Neighbors; ++j) {
        std::vector<CombatLevel> neighbor = compute_neighbor(current, &random);
        double neighbor_value = evaluate(opponents, neighbor);

        if (candidate.empty() || neighbor_value <candidate_value) {
          candidate = std::move(neighbor);
          candidate_value = neighbor_value;
        }
      }

      current = std::move(candidate);
      current_value = candidate_value;

      if (current_value < best_value) {
        best = current;
        best_value = current_value;
      }

      std::println("Iteration: {}, current: {}, best: {}", i, current_value, best_value);
    }

    for (const CombatLevel& level : best) {
      std::println("Attack: {}, Defense: {}", level.attack, level.defense);
    }

  }

}

int main()
{
  dummy2();
}
