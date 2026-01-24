#ifndef FW_COLOR_UTILS_H
#define FW_COLOR_UTILS_H

#include <cstdint>

#include <string_view>

namespace fw {

  uint32_t to_rbga(std::string_view raw);

}

#endif // FW_COLOR_UTILS_H
