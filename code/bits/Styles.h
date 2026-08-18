#ifndef FW_STYLES_H
#define FW_STYLES_H

#include <gf2/core/ConsoleStyle.h>

namespace fw {

  const gf::ConsoleStyle& message_default_style();
  const gf::ConsoleRichStyle& message_rich_style();

  const gf::ConsoleStyle& ui_default_style();
  const gf::ConsoleRichStyle& ui_rich_style();

}

#endif // FW_STYLES_H
