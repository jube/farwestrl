#ifndef FW_WORLD_DATA_H
#define FW_WORLD_DATA_H

#include "ActorData.h"
#include "DataLexicon.h"
#include "ItemData.h"
#include "ThingData.h"

namespace fw {

  struct WorldData {
    DataLexicon<ActorData> actors;
    DataLexicon<ItemData> items;
    DataLexicon<ThingData> things;

    void load_from_file(const std::filesystem::path& filename);

  };

}

#endif // FW_WORLD_DATA_H
