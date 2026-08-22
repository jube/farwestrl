#ifndef FW_ACTOR_DATA_H
#define FW_ACTOR_DATA_H

#include <nlohmann/json.hpp>

#include <gf2/core/Color.h>
#include <gf2/core/TaggedVariant.h>

#include "BodyData.h"
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

  struct HumanElement {
    DisplayData display;
    BodyData body;
  };

  struct AnimalElement {
    DisplayData display;
    BodyData body;
    MapCellBiome biome;
    bool can_be_mounted;
    bool can_idle;
  };

  struct GroupElement {
  };

  struct TrainElement {
  };

  using ActorElement = gf::TaggedVariant<ActorType, HumanElement, AnimalElement, GroupElement, TrainElement>;

  struct ActorData {
    DataLabel label;
    ActorElement element;

    ActorType type() const { return element.type(); }
  };

  void from_json(const nlohmann::json& json, ActorData& data);

}

#endif // FW_ACTOR_DATA_H
