#include <cstdlib>

#include <print>

#include <gf2/core/Image.h>

constexpr int CellCount = 20;
constexpr gf::Color LineColor = gf::White;

namespace {

  int compute_offset(const char* raw, int cell_size)
  {
    return ((std::atoi(raw) * cell_size) / 100) % cell_size;
  }

}

int main(int argc, const char* argv[])
{
  if (argc < 3) {
    std::println("Usage: image-helper <input> <output> [offset_x%] [offset_y%]");
    return 0;
  }

  gf::Image image(argv[1]);
  const gf::Vec2I size = image.size();

  const int max_size = std::max(size.w, size.h);
  const int cell_size = max_size / CellCount;
  const int offset_x = argc > 3 ? compute_offset(argv[3], cell_size) : (size.w % cell_size) / 2;
  const int offset_y = argc > 4 ? compute_offset(argv[4], cell_size) : (size.y % cell_size) / 2;

  for (int x = offset_x; x < size.w; x += cell_size) {
    for (int y = 0; y < size.y; ++y) {
      image.put_pixel({ x, y }, LineColor);
    }
  }

  for (int y = offset_y; y < size.h; y += cell_size) {
    for (int x = 0; x < size.w; ++x) {
      image.put_pixel({ x, y }, LineColor);
    }
  }

  image.save_to_file(argv[2]);
  return 0;
}
