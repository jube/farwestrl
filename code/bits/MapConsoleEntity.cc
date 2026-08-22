#include "MapConsoleEntity.h"

#include <cstdint>
#include <string_view>

#include <gf2/core/ConsoleOperations.h>
#include <gf2/core/Direction.h>

#include "ActorData.h"
#include "ActorState.h"
#include "Colors.h"
#include "FarWest.h"
#include "Index.h"
#include "MapCell.h"
#include "MapRuntime.h"
#include "MapState.h"
#include "Pictures.h"
#include "Settings.h"
#include "Utils.h"

namespace fw {

  namespace {

    constexpr int32_t ViewRelaxation = 5;

    using namespace std::literals;

    constexpr std::array TrainPicture = {
      u"◢█◣"sv,
      u"▐◘▌"sv,
      u"▐█▌"sv,
      u"▐█▌"sv,
      u"███"sv,
      u"███"sv,
      u"◥█◤"sv,
      u" ║ "sv,
      u"◢█◣"sv,
      u"███"sv,
      u"███"sv,
      u"███"sv,
      u"███"sv,
      u"███"sv,
      u"███"sv,
      u"◥█◤"sv,
      u" ║ "sv,
      u"◢█◣"sv,
      u"███"sv,
      u"███"sv,
      u"███"sv,
      u"███"sv,
      u"███"sv,
      u"███"sv,
      u"◥█◤"sv,
      u" ║ "sv,
      u"◢█◣"sv,
      u"███"sv,
      u"███"sv,
      u"███"sv,
      u"███"sv,
      u"███"sv,
      u"███"sv,
      u"◥█◤"sv,
      u"   "sv
    };

    float compute_fade_ratio(gf::Vec2I position, gf::Vec2I hero_position, int32_t min_distance, int32_t max_distance)
    {
      const int32_t square_distance = gf::square_distance(position, hero_position);

      if (square_distance <= gf::square(min_distance)) {
        return 0.0f;
      }

      const int32_t distance = gf::isqrt(square_distance);
      return gf::clamp(static_cast<float>(distance - min_distance) / static_cast<float>(max_distance - min_distance), 0.0f, 1.0f);
    }

  }

  MapConsoleEntity::MapConsoleEntity(FarWest* game)
  : m_game(game)
  {
  }

  void MapConsoleEntity::update([[maybe_unused]] gf::Time time)
  {
    const WorldState* state = m_game->state();
    WorldRuntime* runtime = m_game->runtime();
    const Location location = state->hero().location();
    runtime->view_center = gf::clamp(runtime->view_center, location.position - ViewRelaxation, location.position + ViewRelaxation);
  }

  void MapConsoleEntity::render(gf::Console& console)
  {
    const ActorState& hero = m_game->state()->hero();
    const Location hero_location = hero.location();

    // get current view

    const WorldRuntime* runtime = m_game->runtime();
    const gf::RectI view = runtime->compute_view();

    // display map background

    const FloorMap& floor_map = runtime->map.from_floor(hero_location.floor);
    gf::console_blit_to(floor_map.console, console, view, GameBoxPosition);

    const WorldState* state = m_game->state();
    const BackgroundMap& map = state->map.from_floor(hero_location.floor);

    for (const gf::Vec2I position : gf::rectangle_range(view)) {
      // TODO: verify the position is in the map or clamp the view

      const MapCell& cell = map(position);
      const gf::Vec2I console_position = position - view.position() + GameBoxPosition;

      constexpr gf::Color LightFadeColor = gf::gray(0.75f);
      constexpr gf::Color DarkFadeColor = gf::gray(0.05f);

      if (cell.visible()) {
        const float fade_ratio = compute_fade_ratio(position, hero_location.position, HeroVisionRange - HeroVisionFadeDistance, HeroVisionRange);
        const gf::Color color = gf::lerp(gf::White, LightFadeColor, fade_ratio);

        gf::console_write_background(console, console_position, color, gf::ConsoleEffect::multiply());
        console(console_position).parts[0].foreground *= color;
      } else if (cell.explored()) {
        gf::console_write_background(console, console_position, LightFadeColor, gf::ConsoleEffect::multiply());
        console(console_position).parts[0].foreground *= LightFadeColor;
      } else {
        assert(!cell.visible());
        gf::console_write_background(console, console_position, DarkFadeColor, gf::ConsoleEffect::multiply());
        console(console_position).parts[0].foreground *= DarkFadeColor;
      }
    }

    // display actors

    auto is_visible = [&](Location location) {
      if (!view.contains(location.position)) {
        return false;
      }

      if (!map(location.position).visible()) {
        return false;
      }

      if (location.floor != hero_location.floor) {
        return false;
      }

      return true;
    };

    for (const ActorState& actor : state->actors) {
      switch (actor.component.type()) {
        case ActorType::None:
          break;
        case ActorType::Human:
        {
          const HumanElement& element = actor.data->element.from<ActorType::Human>();
          const HumanComponent& component = actor.component.from<ActorType::Human>();

          if (!is_visible(component.location)) {
            continue;
          }

          gf::console_write_picture(console, component.location.position - view.position(), element.display.picture, element.display.color);
          break;
        }
        case ActorType::Animal:
        {
          const AnimalElement& element = actor.data->element.from<ActorType::Animal>();
          const AnimalComponent& component = actor.component.from<ActorType::Animal>();

          if (!is_visible(component.location)) {
            continue;
          }

          if (component.mounted_by != NoIndex) {
            const ActorState& mounted_by = state->actors[component.mounted_by];
            assert(mounted_by.component.type() == ActorType::Human);
            const HumanElement& human_data = mounted_by.data->element.from<ActorType::Human>();

            gf::ConsoleStyle mounted_style;
            mounted_style.color.foreground = element.display.color;
            mounted_style.color.background = human_data.display.color * gf::opaque(0.2f);
            mounted_style.effect = gf::ConsoleEffect::alpha();
            gf::console_write_picture(console, component.location.position - view.position(), to_uppercase_ascii(element.display.picture), mounted_style);
          } else {
            gf::console_write_picture(console, component.location.position - view.position(), element.display.picture, element.display.color);
          }

          break;
        }
        case ActorType::Group:
        {

          break;
        }
        case ActorType::Train:
        {
          if (hero_location.floor != Floor::Ground) {
            continue;
          }

          const TrainComponent& component = actor.component.from<ActorType::Train>();

          uint32_t offset = 0;
          gf::Vec2I position = { 0, 0 };
          gf::Direction direction = gf::Direction::Center;
          gf::Vec2I step = { 0, 0 };

          for (const std::u16string_view part : TrainPicture) {
            const uint32_t index = runtime->network.next_position(component.railway_index, offset);
            assert(index < runtime->network.railway.size());

            if (direction == gf::Direction::Center || index % 3 == 2) {
              position = runtime->network.railway[index];

              const uint32_t prev_index = runtime->network.prev_position(index);
              assert(prev_index < runtime->network.railway.size());
              const gf::Vec2I prev_position = runtime->network.railway[prev_index];

              assert(gf::manhattan_distance(position, prev_position) == 1);

              direction = undisplacement(prev_position - position);
              step = position - prev_position;
            }

            const gf::Vec2I position0 = position - view.position() + gf::perp(step);

            if (GameBox.contains(position0)) {
              const char16_t picture0 = rotate_picture(part[0], direction);
              gf::console_write_picture(console, position0, picture0, TrainColor);
            }

            const gf::Vec2I position1 = position - view.position();

            if (GameBox.contains(position1)) {
              const char16_t picture1 = rotate_picture(part[1], direction);
              gf::console_write_picture(console, position1, picture1, TrainColor);
            }

            const gf::Vec2I position2 = position - view.position() - gf::perp(step);

            if (GameBox.contains(position2)) {
              const char16_t picture2 = rotate_picture(part[2], direction);
              gf::console_write_picture(console, position2, picture2, TrainColor);
            }

            position -= gf::displacement(direction);
            ++offset;
          }

          break;
        }
      }
    }
  }

}
