#ifndef FW_ACTOR_H
#define FW_ACTOR_H

#include <cstdint>

#include <gf2/core/Color.h>
#include <gf2/core/Fixed.h>
#include <gf2/core/TaggedVariant.h>
#include <gf2/core/Vec2.h>

#include "ActorData.h"
#include "BodyState.h"
#include "Combat.h"
#include "DataReference.h"
#include "Date.h"
#include "Index.h"
#include "InventoryState.h"
#include "Location.h"

namespace fw {

  enum class Gender : uint8_t {
    Girl,
    Boy,
    NonBinary,
  };

  struct HumanFeature {
    std::string name;
    Location location;
    Gender gender;
    MonthDay birthday;
    int8_t age;
    BodyState body;
    uint32_t mounting = NoIndex;
    InventoryState inventory;

    WeaponItemState weapon;
    InventoryItemState projectile;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<HumanFeature, Archive>& feature)
  {
    return ar | feature.name | feature.location | feature.gender | feature.birthday | feature.age | feature.body | feature.mounting | feature.inventory | feature.weapon | feature.projectile;
  }

  struct AnimalFeature {
    Location location;
    BodyState body;
    uint32_t mounted_by = NoIndex;
    InventoryState inventory;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<AnimalFeature, Archive>& feature)
  {
    return ar | feature.location | feature.body | feature.mounted_by | feature.inventory;
  }

  enum class GroupType : uint8_t {
    Herd, // for herbivores
    Pack, // for carnivores
  };

  struct GroupFeature {
    GroupType type;
    std::vector<uint32_t> members;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<GroupFeature, Archive>& feature)
  {
    return ar | feature.type | feature.members;
  }

  struct TrainFeature {
    uint32_t railway_index;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<TrainFeature, Archive>& feature)
  {
    return ar | feature.railway_index;
  }

  using ActorFeature = gf::TaggedVariant<ActorType, HumanFeature, AnimalFeature, GroupFeature, TrainFeature>;

  struct ActorState {
    DataReference<ActorData> data;
    ActorFeature feature;

    ActorType type() const { return feature.type(); }
    Location location() const;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<ActorState, Archive>& state)
  {
    return ar | state.data | state.feature;
  }

  AttackDigest compute_attack_digest(const ActorState& actor);
  DefenseDigest compute_defense_digest(const ActorState& actor);

}

#endif // FW_ACTOR_H
