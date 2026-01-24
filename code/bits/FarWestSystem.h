#ifndef FW_FAR_FAR_WEST_SYSTEM_H
#define FW_FAR_FAR_WEST_SYSTEM_H

#include <filesystem>
#include <memory>

#include <gf2/framework/SceneSystem.h>

#include "FarWestResources.h"
#include "FarWestScene.h"

namespace fw {

  class FarWestSystem : public gf::SceneSystem {
  public:
    FarWestSystem(const std::filesystem::path& asset_directory);

  private:
    FarWestResources m_resources;
    std::unique_ptr<FarWestScene> m_scene;
  };

}

#endif // FW_FAR_FAR_WEST_SYSTEM_H
