#include "ThingData.h"

namespace fw {

  NLOHMANN_JSON_SERIALIZE_ENUM( ThingType, {
    { ThingType::None, nullptr },
    { ThingType::BankDesk, "bank_desk" },
    { ThingType::Bed, "bed" },
    { ThingType::BlackjackTable, "blackjack_table" },
    { ThingType::HotelDesk, "hotel_desk" },
    { ThingType::MarshalDesk, "marshal_desk" },
    { ThingType::PrayerChair, "prayer_chair" },
    { ThingType::SaloonChair, "saloon_chair" },
    { ThingType::SaloonDesk, "saloon_desk" },
    { ThingType::ShopDesk, "shop_desk" },
    { ThingType::SlotMachine, "slot_machine" },
  })

  NLOHMANN_JSON_SERIALIZE_ENUM( ThingQuality, {
    { ThingQuality::Best, "best" },
    { ThingQuality::Good, "good" },
    { ThingQuality::Bad, "bad" },
    { ThingQuality::Worse, "worse" },
  })

  void from_json(const nlohmann::json& json, ThingData& data)
  {
    json.at("label").get_to(data.label);
    json.at("type").get_to(data.type);
    json.at("quality").get_to(data.quality);
    json.at("display").get_to(data.display);
  }

}
