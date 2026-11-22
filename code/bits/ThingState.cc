#include "ThingState.h"

namespace fw {

  std::array<Action, 2> actions_from_thing(const ThingState& thing)
  {
    assert(thing.data);

    switch (thing.data->type) {
      case ThingType::None:
        return { };
      case ThingType::BankDesk:
        // TODO: get money
        break;
      case ThingType::Bed:
        // TODO: rest and sleep
        break;
      case ThingType::BlackjackTable:
        // TODO: play blackjack
        break;
      case ThingType::PrayerChair:
        // TODO: pray
        break;
      case ThingType::SaloonChair:
        // TODO: drink
        break;
      case ThingType::SaloonDesk:
        // TODO: drink and talk
        break;
      case ThingType::ShopDesk:
        // TODO: buy things
        break;
      case ThingType::SlotMachine:
        // play slot machine
        break;
    }

    return { };
  }

}
