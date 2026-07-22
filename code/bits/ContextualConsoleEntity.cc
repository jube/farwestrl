#include "ContextualConsoleEntity.h"

#include <gf2/core/ConsoleOperations.h>

#include "FarWest.h"
#include "Index.h"
#include "Settings.h"
#include "WorldRuntime.h"

namespace fw {

  ContextualConsoleEntity::ContextualConsoleEntity(FarWest* game)
  : m_game(game)
  {
  }

  void ContextualConsoleEntity::update([[maybe_unused]] gf::Time time)
  {
    const WorldState* state = m_game->state();
    const WorldRuntime* runtime = m_game->runtime();
    const Date current_date = state->current_date;

    if (m_latest_update == current_date) {
      return;
    }

    if (!state->scheduler.is_hero_turn()) {
      return;
    }

    const gf::Vec2I hero_position = state->hero().position;
    const gf::RectI view_zone = gf::RectI::from_center_size(hero_position, { 2 * HeroVisionRange, 2 * HeroVisionRange });

    const Floor floor = state->hero().floor;
    const BackgroundMap& background_map = state->map.from_floor(floor);
    const FloorMap& floor_map = runtime->map.from_floor(floor);

    // actors

    m_actors.clear();

    for (const gf::Vec2I position : gf::rectangle_range(view_zone)) {
      if (!background_map(position).visible()) {
        continue;
      }

      const ReverseMapCell& cell = floor_map.reverse(position);

      if (cell.actor_index == NoIndex || cell.actor_index == HeroIndex) {
        continue;
      }

      assert(cell.actor_index < state->actors.size());
      const ActorState& actor_state = state->actors[cell.actor_index];

      if (actor_state.floor != floor) {
        continue;
      }

      const ActorType actor_type = actor_state.feature.type();

      if (actor_type != ActorType::Human && actor_type != ActorType::Animal) {
        continue;
      }

      const ActorData* actor_data = actor_state.data.origin;

      const Element element = {
        .picture = actor_data->picture,
        .name = actor_data->label.tag,
        .foreground = actor_data->color,
        .background = floor_map.console(position).parts[0].background,
        .position = position,
        .distance = static_cast<int32_t>(gf::euclidean_distance(hero_position, position))
      };

      m_actors.push_back(element);
    }

    std::ranges::sort(m_actors, {}, &Element::distance);
    gf::Log::debug("actors nearby: {}", m_actors.size());

    // TODO: same with items

    m_latest_update = current_date;
  }

  void ContextualConsoleEntity::render(gf::Console& console)
  {
    [[maybe_unused]] WorldState* state = m_game->state();

    gf::ConsoleStyle contextual_box_style;
    contextual_box_style.color.foreground = gf::Gray;
    gf::console_draw_frame(console, ContextualBox, contextual_box_style);

    constexpr gf::Vec2I ContextualContentSize = ContextualBoxSize - 2;
    constexpr int32_t ScanHeight = ContextualContentSize.y / 2;

    gf::Console contextual_console(ContextualContentSize);

    int32_t count = 0;
    gf::Vec2I position = { 0, 0 };

    for (const Element& element : m_actors) {
      gf::console_write_picture(contextual_console, position + gf::diry(count), element.picture, { element.foreground, element.background });
      gf::console_print_text(contextual_console, position + gf::diry(count) + gf::dirx(2), gf::ConsoleAlignment::Left, m_game->style(), "{} ({}m)", element.name, element.distance);

      ++count;

      if (count >= ScanHeight) {
        break;
      }
    }

    gf::console_blit_to(contextual_console, console, ContextualBoxPosition + 1);
  }

}
