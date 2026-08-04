#include "ActorState.h"
#include "ActorData.h"
#include "Combat.h"
#include "ItemData.h"
#include "ItemState.h"

namespace fw {

  Location ActorState::location() const
  {
    const ActorType actor_type = type();

    if (actor_type == ActorType::Human) {
      return feature.from<ActorType::Human>().location;
    }

    if (actor_type == ActorType::Animal) {
      return feature.from<ActorType::Animal>().location;
    }

    return {};
  }

  AttackDigest compute_attack_digest(const ActorState& actor)
  {
    AttackDigest digest;
    digest.modifier = 0;

    switch (actor.type()) {
      case ActorType::None:
      case ActorType::Group:
      case ActorType::Train:
        assert(false);
        break;
      case ActorType::Human:
      {
        const HumanFeature& feature = actor.feature.from<ActorType::Human>();
        digest.luck = feature.body.luck;

        if (!feature.weapon.data) {
          // use your fist
          digest.attribute = feature.body.force;
          digest.attack = feature.body.attack;
          digest.time = feature.body.attack_time;
          digest.range = 0;
        } else {
          switch (feature.weapon.data->feature.type()) {
            case ItemType::None:
            case ItemType::Projectile:
              assert(false);
              break;
            case ItemType::MeleeWeapon:
            {
              digest.attribute = feature.body.force;

              const MeleeWeaponDataFeature& weapon = feature.weapon.data->feature.from<ItemType::MeleeWeapon>();
              digest.attack = weapon.attack;
              digest.time = weapon.use_time;
              digest.modifier += weapon.modifier;
              digest.range = 0;
              break;
            }
            case ItemType::DistanceWeapon:
            {
              digest.attribute = feature.body.dexterity;

              const DistanceWeaponDataFeature& weapon = feature.weapon.data->feature.from<ItemType::DistanceWeapon>();
              digest.range = weapon.range;
              digest.modifier += weapon.modifier;
              digest.time = weapon.shoot_time;

              if (feature.projectile.data && feature.weapon.projectiles > 0) {
                assert(feature.projectile.data->feature.type() == ItemType::Projectile);
                const ProjectileDataFeature& ammunition = feature.projectile.data->feature.from<ItemType::Projectile>();
                digest.attack = ammunition.attack;
                digest.modifier += ammunition.modifier;
              } else {
                digest.attack = 0;
                digest.time = 1; // TODO: maybe define a constant in this case or use shoot_time
              }
              break;
            }
          }
        }

        break;
      }
      case ActorType::Animal:
      {
        const AnimalFeature& feature = actor.feature.from<ActorType::Animal>();
        digest.attribute = feature.body.force;
        digest.luck = feature.body.luck;
        digest.attack = feature.body.attack;
        digest.time = feature.body.attack_time;
        digest.range = 0;
        break;
      }
    }

    return digest;
  }

  DefenseDigest compute_defense_digest(const ActorState& actor)
  {
    DefenseDigest digest;

    switch (actor.type()) {
      case ActorType::None:
      case ActorType::Group:
      case ActorType::Train:
        assert(false);
        break;
      case ActorType::Human:
      {
        const HumanFeature& feature = actor.feature.from<ActorType::Human>();
        digest.defense = feature.body.defense;

        // TODO: clothes
        break;
      }
      case ActorType::Animal:
      {
        const AnimalFeature& feature = actor.feature.from<ActorType::Animal>();
        digest.defense = feature.body.defense;
        break;
      }
    }

    return digest;
  }


}
