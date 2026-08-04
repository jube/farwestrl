#include "ActorGeneration.h"

#include <cassert>

#include <cstdint>
#include <gf2/core/Dice.h>

#include "Date.h"
#include "Names.h"
#include "WorldData.h"

namespace fw {

  namespace {

    constexpr int32_t StatMin = 50;
    constexpr int32_t StatMax = 90;

    int8_t generate_attribute(std::string_view spec, gf::Random* random)
    {
      gf::Dice dice(spec);
      return static_cast<int8_t>(dice.roll(random));
    }

    BodyState generate_body(const BodyData& data, gf::Random* random)
    {
      BodyState state = {};
      state.max_health = state.health = data.max_health;

      state.force = generate_attribute(data.force, random);
      state.dexterity = generate_attribute(data.dexterity, random);
      state.constitution = generate_attribute(data.constitution, random);
      state.luck = generate_attribute(data.luck, random);

      state.intensity = random->compute_uniform_integer(StatMin, StatMax);
      state.precision = random->compute_uniform_integer(StatMin, StatMax);
      state.endurance = random->compute_uniform_integer(StatMin, StatMax);

      state.attack_time = data.attack_time;
      state.attack = generate_attribute(data.attack, random);
      state.defense = generate_attribute(data.defense, random);

      return state;
    }

    Gender generate_gender(gf::Random* random)
    {
      std::discrete_distribution distribution({ 50.0, 48.0, 2.0 });
      const uint8_t index = static_cast<uint8_t>(distribution(random->engine()));
      return static_cast<Gender>(index);
    }

  }

  ActorState generate_animal(std::string_view tag, Location location, const WorldData& world_data, gf::Random* random)
  {
    ActorState actor;
    actor.data = tag;
    actor.data.bind_from(world_data.actors);

    assert(actor.data->feature.type() == ActorType::Animal);
    const AnimalDataFeature& data = actor.data->feature.from<ActorType::Animal>();

    AnimalFeature feature;
    feature.location = location;
    feature.body = generate_body(data.body, random);
    feature.mounted_by = NoIndex;

    actor.feature = feature;
    return actor;
  }

  ActorState generate_human(std::string_view tag, Location location, const WorldData& world_data, gf::Random* random)
  {
    ActorState actor;
    actor.data = tag;
    actor.data.bind_from(world_data.actors);

    assert(actor.data->feature.type() == ActorType::Human);
    const HumanDataFeature& data = actor.data->feature.from<ActorType::Human>();

    HumanFeature feature;
    feature.location = location;
    feature.gender = generate_gender(random);
    feature.birthday = generate_random_birthday(random);
    feature.age = random->compute_uniform_integer<int8_t>(20, 40);;

    switch (feature.gender) {
      case Gender::Girl:
        feature.name = generate_random_white_female_name(random);
        break;
      case Gender::Boy:
        feature.name = generate_random_white_male_name(random);
        break;
      case Gender::NonBinary:
        feature.name = generate_random_white_non_binary_name(random);
        break;
    }

    feature.body = generate_body(data.body, random);

    gf::Log::info("Name: {} (Luck: {})", feature.name, feature.body.luck);

    actor.feature = feature;
    return actor;
  }

}


