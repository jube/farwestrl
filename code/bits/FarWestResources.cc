#include "FarWestResources.h"

#include <gf2/framework/BundleBuilder.h>

#include "FarWestSystem.h"

namespace fw {

  FarWestResources::FarWestResources()
  {
    console_resource.console_font = "Hack_square_64x64.png";
    console_resource.data.font_format = gf::DwarfFortressFormat;
  }

  gf::ResourceBundle FarWestResources::bundle(FarWestSystem* game) const
  {
    gf::BundleBuilder builder(game);

    builder.add_in_bundle(console_resource);

    return builder.make_bundle();

  }

}
