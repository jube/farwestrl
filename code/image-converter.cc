#include <fstream>
#include <print>

#include <nlohmann/json.hpp>

#include <gf2/core/ConsoleFontData.h>
#include <gf2/core/StringUtils.h>

constexpr std::size_t Size = 20;

int main(int argc, const char* argv[])
{
  if (argc != 3) {
    std::println("Usage: image-converter <input> <output>");
    return 0;
  }

  std::ifstream input(argv[1]);

  if (!input) {
    std::println("Error: input file does not exist.");
    return 0;
  }

  std::ofstream output(argv[2]);

  gf::Span<const gf::ConsoleFontElement> font_mapping = gf::load_console_font_mapping(gf::ConsoleFontMapping::Picture);

  const nlohmann::json json = nlohmann::json::parse(input);
  const nlohmann::json tiles = json.at("frames").at(0).at("layers").at(0).at("tiles");

  std::u32string line;

  for (const nlohmann::json& tile : tiles) {
    std::size_t index = 0;
    tile.at("char").get_to(index);
    assert(index <font_mapping.size());
    const gf::ConsoleFontElement& element = font_mapping[index];
    assert(index == element.index);

    line.push_back(static_cast<char32_t>(element.character));

    if (line.size() == Size) {
      const std::string string = gf::to_utf8(line);
      output << string << '\n';
      line.clear();
    }
  }

  return 0;
}
