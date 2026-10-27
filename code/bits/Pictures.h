#ifndef FW_CHARACTERS_H
#define FW_CHARACTERS_H

#include <gf2/core/Direction.h>

namespace fw {

  constexpr char16_t HorizontalDoorPicture = u'\u2550'; /* '═' */
  constexpr char16_t VerticalDoorPicture = u'\u2551'; /* '║' */

  char16_t rotate_picture(char16_t picture, gf::Direction direction);

}

#endif // FW_CHARACTERS_H
