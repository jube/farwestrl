#ifndef FW_ACTOR_DATA_H
#define FW_ACTOR_DATA_H

#include <nlohmann/json.hpp>

#include <gf2/core/Color.h>
#include <gf2/core/TaggedVariant.h>

#include "BodyData.h"
#include "Combat.h"
#include "DataLabel.h"
#include "DisplayData.h"
#include "MapCellBiome.h"

namespace fw {

  enum class ActorType : uint16_t {
    None,
    Human,
    Animal,
    Group,
    Train,
  };

  struct HumanDataFeature {
    DisplayData display;
    BodyData body;
  };

  struct AnimalDataFeature {
    DisplayData display;
    BodyData body;
    MapCellBiome biome;
    bool can_be_mounted;
    bool can_idle;
  };

  struct GroupDataFeature {
  };

  struct TrainDataFeature {
  };

  using ActorDataFeature = gf::TaggedVariant<ActorType, HumanDataFeature, AnimalDataFeature, GroupDataFeature, TrainDataFeature>;

  struct ActorData {
    DataLabel label;
    ActorDataFeature feature;

    ActorType type() const { return feature.type(); }
  };

  void from_json(const nlohmann::json& json, ActorData& data);

}

#endif // FW_ACTOR_DATA_H
