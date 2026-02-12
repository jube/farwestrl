#ifndef FW_FAR_FAR_WEST_RESOURCES_H
#define FW_FAR_FAR_WEST_RESOURCES_H

#include <gf2/core/ConsoleFontResource.h>
#include <gf2/core/ResourceBundle.h>

namespace fw {
  class FarWestSystem;

  struct FarWestResources {
    FarWestResources();

    gf::ResourceBundle bundle(FarWestSystem* game) const;

    gf::ConsoleFontResource console_resource;
  };

}

#endif // FW_FAR_FAR_WEST_RESOURCES_H
