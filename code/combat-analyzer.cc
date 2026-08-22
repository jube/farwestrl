#include <map>
#include <print>
#include <ranges>
#include <stdexcept>

#include <gf2/core/Dice.h>
#include <gf2/core/Random.h>

#include "bits/ActorData.h"
#include "bits/Combat.h"
#include "bits/ItemData.h"
#include "bits/WorldData.h"

#include "config.h"

constexpr int LoCount = 20;
constexpr int HiCount = 100000;

constexpr double MinLevel = 10.0;
constexpr double MaxLevel = 100.0;

constexpr std::size_t Iterations = 10000;
constexpr std::size_t Neighbors = 20;

int dummy()
{
  const fw::AttackDigest attacker = {
    .attribute = 13,
    .modifier = 0,
    .luck = 10,
    .time = 10,
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
  struct Named {
    std::string name;
    T data;
  };

  template<typename T>
  Named<T> find_or_throw(const std::map<std::string, T, std::less<>>& map, std::string_view name, const char* what)
  {
    if (auto iterator = map.find(name); iterator != map.end()) {
      return { std::string(name), iterator->second };
    }

    throw std::runtime_error(what);
  }

  struct Weapons {
    std::map<std::string, fw::MeleeWeaponDataFeature, std::less<>> melee_weapons;
    std::map<std::string, fw::DistanceWeaponDataFeature, std::less<>> distance_weapons;
    std::map<std::string, fw::ProjectileDataFeature, std::less<>> projectiles;

    Named<fw::MeleeWeaponDataFeature> melee_weapon(std::string_view name) const
    {
      return find_or_throw(melee_weapons, name, "Unknown melee weapon");
    }

    Named<fw::DistanceWeaponDataFeature> distance_weapon(std::string_view name) const
    {
      return find_or_throw(distance_weapons, name, "Unknown distance weapon");
    }

    Named<fw::ProjectileDataFeature> projectile(std::string_view name) const
    {
      return find_or_throw(projectiles, name, "Unknown projectile");
    }
  };

  Weapons compute_weapons(const fw::WorldData& data)
  {
    Weapons weapons;

    for (const fw::ItemData& item : data.items) {
      switch (item.type()) {
        case fw::ItemType::None:
        case fw::ItemType::Container:
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

    Named<fw::HumanDataFeature> human(std::string_view name) const
    {
      return find_or_throw(humans, name, "Unknown human");
    }

    Named<fw::AnimalDataFeature> animal(std::string_view name) const
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
    std::string name;
    double health;
    double attribute;
    double luck;
    double range;
    double modifier;
    double attack_time;
    double attack;
    double defense;
  };

  double compute_average_from_dice(std::string_view spec)
  {
    gf::Dice dice(spec);
    return (static_cast<double>(dice.faces()) + 1) / 2.0 * dice.count() + dice.modifier();
  }

  CombatDigest compute_combat_digest(const Named<fw::AnimalDataFeature>& animal)
  {
    return {
      .name = animal.name,
      .health = static_cast<double>(animal.data.body.max_health),
      .attribute = compute_average_from_dice(animal.data.body.force),
      .luck = compute_average_from_dice(animal.data.body.luck),
      .range = 0.0,
      .modifier = 0.0,
      .attack_time = static_cast<double>(animal.data.body.attack_time),
      .attack = compute_average_from_dice(animal.data.body.attack),
      .defense = compute_average_from_dice(animal.data.body.defense),
    };
  }

  CombatDigest compute_combat_digest(const Named<fw::HumanDataFeature>& human, const Named<fw::MeleeWeaponDataFeature>& weapon)
  {
    return {
      .name = human.name + " with " + weapon.name,
      .health = static_cast<double>(human.data.body.max_health),
      .attribute = compute_average_from_dice(human.data.body.force),
      .luck = compute_average_from_dice(human.data.body.luck),
      .range = 0.0,
      .modifier = static_cast<double>(weapon.data.modifier),
      .attack_time = static_cast<double>(weapon.data.use_time),
      .attack = weapon.data.attack.as_float(),
      .defense = compute_average_from_dice(human.data.body.defense),
    };
  }

  CombatDigest compute_combat_digest(const Named<fw::HumanDataFeature>& human, const Named<fw::DistanceWeaponDataFeature>& weapon, const Named<fw::ProjectileDataFeature>& projectile)
  {
    return {
      .name = human.name + " with " + weapon.name,
      .health = static_cast<double>(human.data.body.max_health),
      .attribute = compute_average_from_dice(human.data.body.dexterity),
      .luck = compute_average_from_dice(human.data.body.luck),
      .range = static_cast<double>(weapon.data.range),
      .modifier = static_cast<double>(weapon.data.modifier),
      .attack_time = static_cast<double>(weapon.data.shoot_time),
      .attack = projectile.data.attack.as_float(),
      .defense = compute_average_from_dice(human.data.body.defense),
    };
  }

  double evaluate_average_damage_points(const CombatDigest& attacker, const CombatDigest& defender)
  {
    const double first_roll_success = std::min(attacker.attribute + attacker.modifier, 20.0) / 20.0;
    assert(first_roll_success <= 1.0);
    const double first_roll_damage = first_roll_success * std::max((1.0 + first_roll_success / 2.0) * attacker.attack - defender.defense, 0.0);
    const double second_roll_success = (1.0 - first_roll_success) * attacker.luck / 20.0;
    assert(second_roll_success <= 1.0);
    const double second_roll_damage = second_roll_success * std::max(0.8 * attacker.attack - defender.defense, 0.0);
    return std::max(first_roll_damage + second_roll_damage, 1.0);
  }

  double evaluate_time_to_beat(const CombatDigest& attacker, const CombatDigest& defender, double damage_points)
  {
    const double walk_time = 0.0; // static_cast<double>(fw::StraightWalkTime) * std::max(defender.range - attacker.range, 0.0);
    const double attack_time = std::ceil(defender.health / damage_points) * attacker.attack_time;
    return walk_time + attack_time;
  }

  using CombatEvaluation = std::array<double, 2>;

  CombatEvaluation evalutate_combat(const CombatDigest& lhs, const CombatDigest& rhs)
  {
    CombatEvaluation evaluation = {};

    const double lhs_damage_points = evaluate_average_damage_points(lhs, rhs);
    evaluation[0] = evaluate_time_to_beat(lhs, rhs, lhs_damage_points);

    // std::println("{}", lhs_damage_points);

    const double rhs_damage_points = evaluate_average_damage_points(rhs, lhs);
    evaluation[1] = evaluate_time_to_beat(rhs, lhs, rhs_damage_points);

    // std::println("{}", rhs_damage_points);

    return evaluation;
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
      compute_combat_digest(actors.animal("Horse")),
      compute_combat_digest(actors.animal("Cow")),
      compute_combat_digest(actors.animal("Coyote")),
      compute_combat_digest(actors.human("Hero"), weapons.melee_weapon("Knife")),
      compute_combat_digest(actors.animal("Grizzli")),
      compute_combat_digest(actors.animal("Bison")),
      compute_combat_digest(actors.human("Hero"), weapons.distance_weapon("Colt Dragoon Revolver"), weapons.projectile(".44 Ammunition"))
    };

    for (const auto & [ lhs, rhs ] : std::ranges::views::cartesian_product(opponents, opponents)) {
      const CombatEvaluation evaluation = evalutate_combat(lhs, rhs);
      std::println("{} vs {}: {:g}s vs {:g}s", lhs.name, rhs.name, evaluation[0], evaluation[1]);
    }

  }

  void dummy3()
  {
    CombatDigest human = {
      .name = "human",
      .health = 20.0,
      .attribute = 12.5,
      .luck = 10.5,
      .range = 0.0,
      .modifier = 0.0,
      .attack_time = 1.0,
      .attack = 0.0,
      .defense = 0.0,
    };

    CombatDigest animal = {
      .name = "animal",
      .health = 20.0,
      .attribute = 12.5,
      .luck = 2.5,
      .range = 0.0,
      .modifier = 0.0,
      .attack_time = 1.0,
      .attack = 0.0,
      .defense = 0.0,
    };

    for (int attack = 5; attack <= 40; attack += 5) {
      human.attack = attack;
      animal.attack = attack;

      for (int defense = 5; defense <= attack; defense += 5) {
        animal.defense = defense;
        human.defense = defense;

        const double ha_damage_points = std::round(evaluate_average_damage_points(human, animal));
        const double ah_damage_points = std::round(evaluate_average_damage_points(animal, human));

        std::println("atk: {:2} / def: {:2} = h/a {:3g} pts | a/h {:3g} pts)", attack, defense, ha_damage_points, ah_damage_points);
      }
    }

  }

  using Fitness = std::pair<double, double>;

  Fitness evaluate(const std::vector<CombatDigest>& solution)
  {
    Fitness value = { 0.0, 0.0 };

    for (const auto& [ i, lhs ] : std::views::enumerate(solution)) {
      for (std::size_t j = i + 1; j < solution.size(); ++j) {
        const CombatDigest& rhs = solution[j];

        const CombatEvaluation evaluation = evalutate_combat(lhs, rhs);
        value.first += (j - i) * std::max(0.0, evaluation[1] - evaluation[0]);
        value.second += (rhs.attack - lhs.attack + lhs.defense - rhs.defense) / (j - i) ;
      }
    }

    return value;
  }

  void compute_random_values(std::vector<CombatDigest>& solution, gf::Random* random)
  {
    for (CombatDigest& digest : solution) {
      if (digest.name.starts_with("Hero")) {
        continue;
      }

      digest.attack = random->compute_uniform_float(MinLevel, MaxLevel);
      digest.defense = random->compute_uniform_float(MinLevel, MaxLevel);
    }
  }


  std::vector<CombatDigest> compute_neighbor(const std::vector<CombatDigest>& solution, gf::Random* random)
  {
    std::vector<CombatDigest> neighbor = solution;
    std::size_t index = random->compute_uniform_integer(solution.size());

    while (neighbor[index].name.starts_with("Hero")) {
      index = random->compute_uniform_integer(solution.size());
    }

    assert(index < solution.size());

    neighbor[index].attack = random->compute_uniform_float(MinLevel, MaxLevel);
    neighbor[index].defense = random->compute_uniform_float(MinLevel, MaxLevel);

    return neighbor;
  }

  void dummy4()
  {
    const std::filesystem::path data_directory = fw::FarWestDataDirectory;

    gf::Random random;

    fw::WorldData data;
    data.load_from_file(data_directory / "data.json");

    Weapons weapons = compute_weapons(data);
    Actors actors = compute_actors(data);

    std::vector<CombatDigest> current_solution = {
      compute_combat_digest(actors.animal("Scorpion")),
      compute_combat_digest(actors.animal("Snake")),
      compute_combat_digest(actors.animal("Horse")),
      compute_combat_digest(actors.animal("Cow")),
      compute_combat_digest(actors.animal("Coyote")),
      compute_combat_digest(actors.human("Hero"), weapons.melee_weapon("Knife")),
      compute_combat_digest(actors.animal("Grizzli")),
      compute_combat_digest(actors.animal("Bison")),
      compute_combat_digest(actors.human("Hero"), weapons.distance_weapon("Colt Dragoon Revolver"), weapons.projectile(".44 Ammunition"))
    };

    compute_random_values(current_solution, &random);

    Fitness current_value = evaluate(current_solution);

    std::vector<CombatDigest> best_solution = current_solution;
    Fitness best_value = current_value;

    for (std::size_t i = 0; i < Iterations; ++i) {
      std::vector<CombatDigest> candidate_solution;
      Fitness candidate_value;

      for (std::size_t j = 0; j < Neighbors; ++j) {
        std::vector<CombatDigest> neighbor_solution = compute_neighbor(current_solution, &random);
        Fitness neighbor_value = evaluate(neighbor_solution);

        if (candidate_solution.empty() || neighbor_value < candidate_value) {
          candidate_solution = std::move(neighbor_solution);
          candidate_value = neighbor_value;
        }
      }

      current_solution = std::move(candidate_solution);
      current_value = candidate_value;

      if (current_value < best_value) {
        best_solution = current_solution;
        best_value = current_value;
      }

      std::println("Iteration: {}, current: ({}, {}), best: ({}, {})", i, current_value.first, current_value.second, best_value.first, best_value.second);
    }

    for (const CombatDigest& digest : best_solution) {
      std::println("{:35}: atk = {:3g}, def = {:3g}", digest.name, std::ceil(digest.attack), std::ceil(digest.defense));
    }

    for (const auto & [ lhs, rhs ] : std::ranges::views::cartesian_product(best_solution, best_solution)) {
      const CombatEvaluation evaluation = evalutate_combat(lhs, rhs);
      std::println("{} vs {}: {:g}s vs {:g}s", lhs.name, rhs.name, evaluation[0], evaluation[1]);
    }

  }

}

int main()
{
  const std::filesystem::path data_directory = fw::FarWestDataDirectory;

  gf::Random random;

  fw::WorldData data;
  data.load_from_file(data_directory / "data.json");

  Weapons weapons = compute_weapons(data);
  Actors actors = compute_actors(data);

  using CombatGroup = std::vector<CombatDigest>;

  const std::vector<CombatGroup> groups {
    {
      // small animals
      compute_combat_digest(actors.animal("Scorpion")),
      compute_combat_digest(actors.animal("Snake")),
    },
    {
      // herbivores or small carnivores
      compute_combat_digest(actors.animal("Horse")),
      compute_combat_digest(actors.animal("Cow")),
      compute_combat_digest(actors.animal("Coyote")),
      compute_combat_digest(actors.human("Hero"), weapons.melee_weapon("Knife")),
    },
    {
      // big animals
      compute_combat_digest(actors.animal("Grizzli")),
      compute_combat_digest(actors.animal("Bison")),
      compute_combat_digest(actors.human("Hero"), weapons.distance_weapon("Colt Dragoon Revolver"), weapons.projectile(".44 Ammunition")),
    },
  };

  // check that inside a group, the hierarchy is globally respected
  std::println("Checking intra-groups...");

  constexpr double Threshold = 7;

  for (const CombatGroup& group : groups) {
    for (const auto& [ weak_actor, strong_actor ] : std::views::adjacent<2>(group)) {
      auto [ weak_time, strong_time ] = evalutate_combat(weak_actor, strong_actor);

      if (weak_time > strong_time + Threshold) {
        std::println("- {},{},{},{}", weak_actor.name, weak_time, strong_actor.name, strong_time);
      }
    }
  }

  // check that each member of a stronger group are really stronger than any member of a weaker group
  std::println("Checking inter-groups...");

  for (const auto& [ weak_group, strong_group ] : std::views::adjacent<2>(groups)) {
    for (const auto& [ weak_actor, strong_actor ] : std::views::cartesian_product(weak_group, strong_group)) {
      auto [ weak_time, strong_time ] = evalutate_combat(weak_actor, strong_actor);

      if (weak_time < strong_time) {
        std::println("- {},{},{},{}", weak_actor.name, weak_time, strong_actor.name, strong_time);
      }
    }
  }

  // print all hero variations versus the rest of the world

  CombatGroup hero_group;

  for (const CombatGroup& group : groups) {
    for (const CombatDigest& actor : group) {
      if (actor.name.starts_with("Hero")) {
        hero_group.push_back(actor);
      }
    }
  }

  for (const CombatDigest& hero : hero_group) {
    std::println("{}", hero.name);

    for (const CombatGroup& group : groups) {
      for (const CombatDigest& actor : group) {
        const CombatEvaluation evaluation = evalutate_combat(hero, actor);
        std::println("- {:35}: {:3g}s / {:3g}s", actor.name, evaluation[0], evaluation[1]);
      }
    }
  }

}
