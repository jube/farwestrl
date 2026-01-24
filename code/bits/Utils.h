#ifndef FW_UTILS_H
#define FW_UTILS_H

#include <gf2/core/Direction.h>

namespace fw {

  char16_t to_uppercase_ascii(char16_t c);

  gf::Direction undisplacement(gf::Vec2I displacement);

}

#endif // FW_UTILS_H
