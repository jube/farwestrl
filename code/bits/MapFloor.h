#ifndef FW_MAP_FLOOR_H
#define FW_MAP_FLOOR_H

#include <cstdint>

namespace fw {

  enum class Floor : int8_t {
    Underground = -1,
    Ground = 0,
    Upstairs = 1,
  };

}

#endif // FW_MAP_FLOOR_H
