#include "FarWestResources.h"

#include <gf2/framework/BundleBuilder.h>

#include "FarWestSystem.h"

namespace fw {

  FarWestResources::FarWestResources()
  {
    console_font_resource.picture.console_font = "DejaVuSansMono-Bold_picture.png";
    console_font_resource.picture.data.font_format = gf::ConsolePictureFormat;

    console_font_resource.text.console_font = "DejaVuSansMono-Bold_text.png";
    console_font_resource.text.data.font_format = gf::ConsoleTextFormat;
  }

  gf::ResourceBundle FarWestResources::bundle(FarWestSystem* game) const
  {
    gf::BundleBuilder builder(game);

    builder.add_in_bundle(console_font_resource);

    return builder.make_bundle();

  }

}
