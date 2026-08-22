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
      return component.from<ActorType::Human>().location;
    }

    if (actor_type == ActorType::Animal) {
      return component.from<ActorType::Animal>().location;
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
        const HumanComponent& human_component = actor.component.from<ActorType::Human>();
        digest.luck = human_component.body.luck;

        if (!human_component.weapon.data) {
          // use your fist
          digest.attribute = human_component.body.force;
          digest.attack = human_component.body.attack;
          digest.time = human_component.body.attack_time;
          digest.range = 0;
        } else {
          switch (human_component.weapon.data->element.type()) {
            case ItemType::None:
            case ItemType::Container:
            case ItemType::Projectile:
              assert(false);
              break;
            case ItemType::MeleeWeapon:
            {
              digest.attribute = human_component.body.force;

              const MeleeWeaponElement& weapon = human_component.weapon.data->element.from<ItemType::MeleeWeapon>();
              digest.attack = weapon.attack;
              digest.time = weapon.use_time;
              digest.modifier += weapon.modifier;
              digest.range = 0;
              break;
            }
            case ItemType::DistanceWeapon:
            {
              digest.attribute = human_component.body.dexterity;

              const DistanceWeaponElement& weapon_element = human_component.weapon.data->element.from<ItemType::DistanceWeapon>();
              digest.range = weapon_element.range;
              digest.modifier += weapon_element.modifier;
              digest.time = weapon_element.shoot_time;

              const DistanceWeaponComponent& weapon_component = human_component.weapon.component.from<ItemType::DistanceWeapon>();

              if (human_component.projectile.data && weapon_component.projectiles > 0) {
                assert(human_component.projectile.data->element.type() == ItemType::Projectile);
                const ProjectileElement& projectile = human_component.projectile.data->element.from<ItemType::Projectile>();
                digest.attack = projectile.attack;
                digest.modifier += projectile.modifier;
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
        const AnimalComponent& component = actor.component.from<ActorType::Animal>();
        digest.attribute = component.body.force;
        digest.luck = component.body.luck;
        digest.attack = component.body.attack;
        digest.time = component.body.attack_time;
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
        const HumanComponent& component = actor.component.from<ActorType::Human>();
        digest.defense = component.body.defense;

        // TODO: clothes
        break;
      }
      case ActorType::Animal:
      {
        const AnimalComponent& component = actor.component.from<ActorType::Animal>();
        digest.defense = component.body.defense;
        break;
      }
    }

    return digest;
  }


}
