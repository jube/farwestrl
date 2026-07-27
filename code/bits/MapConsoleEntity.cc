#include "MapConsoleEntity.h"

#include <cstdint>
#include <string_view>

#include <gf2/core/ConsoleOperations.h>
#include <gf2/core/Direction.h>

#include "ActorState.h"
#include "FarWest.h"
#include "MapCell.h"
#include "MapRuntime.h"
#include "MapState.h"
#include "NetworkState.h"
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

  }

  MapConsoleEntity::MapConsoleEntity(FarWest* game)
  : m_game(game)
  {
  }

  void MapConsoleEntity::update([[maybe_unused]] gf::Time time)
  {
    const WorldState* state = m_game->state();
    WorldRuntime* runtime = m_game->runtime();
    const gf::Vec2I hero_position = state->hero().position;
    runtime->view_center = gf::clamp(runtime->view_center, hero_position - ViewRelaxation, hero_position + ViewRelaxation);
  }

  void MapConsoleEntity::render(gf::Console& console)
  {
    const ActorState& hero = m_game->state()->hero();

    // get current view

    const WorldRuntime* runtime = m_game->runtime();
    const gf::RectI view = runtime->compute_view();

    // display map background

    const FloorMap& floor_map = runtime->map.from_floor(hero.floor);
    gf::console_blit_to(floor_map.console, console, view, GameBoxPosition);

    const WorldState* state = m_game->state();
    const BackgroundMap& map = state->map.from_floor(hero.floor);

    for (const gf::Vec2I position : gf::rectangle_range(view)) {
      // TODO: verify the position is in the map or clamp the view

      const MapCell& cell = map(position);

      if (cell.visible()) {
        continue;
      }

      const gf::Vec2I console_position = position - view.position() + GameBoxPosition;

      if (cell.explored()) {
        constexpr gf::Color LightFadeColor = gf::gray(0.75f);
        gf::console_write_background(console, console_position, LightFadeColor, gf::ConsoleEffect::multiply());
        console(console_position).parts[0].foreground *= LightFadeColor;
      } else {
        assert(!cell.visible());
        constexpr gf::Color DarkFadeColor = gf::gray(0.05f);
        gf::console_write_background(console, console_position, DarkFadeColor, gf::ConsoleEffect::multiply());
        console(console_position).parts[0].foreground *= DarkFadeColor;
      }
    }

    // display actors

    gf::ConsoleStyle actor_style;

    for (const ActorState& actor : state->actors) {
      if (!view.contains(actor.position)) {
        continue;
      }

      if (!map(actor.position).visible()) {
        continue;
      }

      if (actor.floor != hero.floor) {
        continue;
      }

      actor_style.color.background = gf::Transparent;
      actor_style.color.foreground = actor.data->color;
      actor_style.effect = gf::ConsoleEffect::none();
      char16_t actor_picture = actor.data->picture;

      if (actor.feature.type() == ActorType::Animal) {
        const uint32_t index = actor.feature.from<ActorType::Animal>().mounted_by;

        if (index != NoIndex) {
          const ActorState& mounted_by = state->actors[index];
          actor_style.color.background = mounted_by.data->color * gf::opaque(0.2f);
          actor_style.effect = gf::ConsoleEffect::alpha();
          actor_picture = to_uppercase_ascii(actor_picture);
        }
      }

      gf::console_write_picture(console, actor.position - view.position(), actor_picture, actor_style);
    }

    // display trains

    if (hero.floor != Floor::Ground) {
      return;
    }

    gf::ConsoleStyle train_style;
    train_style.color.foreground = gf::gray(0.1f);
    train_style.color.background = gf::Transparent;
    train_style.effect = gf::ConsoleEffect::none();

    for (const TrainState& train : state->network.trains) {
      uint32_t offset = 0;
      gf::Vec2I picture_position = { 0, 0 };
      gf::Direction direction = gf::Direction::Center;
      gf::Vec2I step = { 0, 0 };

      for (const std::u16string_view part : TrainPicture) {
        const uint32_t index = runtime->network.next_position(train.railway_index, offset);
        assert(index < runtime->network.railway.size());
        const gf::Vec2I position = runtime->network.railway[index];

        const uint32_t prev_index = runtime->network.prev_position(index);
        assert(prev_index < runtime->network.railway.size());
        const gf::Vec2I prev_position = runtime->network.railway[prev_index];

        assert(gf::manhattan_distance(position, prev_position) == 1);

        if (direction == gf::Direction::Center || index % 3 == 2) {
          direction = undisplacement(prev_position - position);
          picture_position = position;
          step = position - prev_position;
        }

        const char16_t picture0 = rotate_picture(part[0], direction);
        const char16_t picture1 = rotate_picture(part[1], direction);
        const char16_t picture2 = rotate_picture(part[2], direction);

        gf::console_write_picture(console, picture_position - view.position() + gf::perp(step), picture0, train_style);
        gf::console_write_picture(console, picture_position - view.position(), picture1, train_style);
        gf::console_write_picture(console, picture_position - view.position() - gf::perp(step), picture2, train_style);

        picture_position -= gf::displacement(direction);
        ++offset;
      }
    }

  }

}
