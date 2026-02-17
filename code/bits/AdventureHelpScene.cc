#include "AdventureHelpScene.h"

#include <gf2/core/ConsoleOperations.h>

#include "Colors.h"
#include "FarWest.h"
#include "Settings.h"

namespace fw {

  namespace {

    constexpr gf::RectI MoveHelpBox = gf::RectI::from_position_size({ 1, 1 }, { 47, 21 });

    constexpr std::string_view MoveHelpText = R"help(
<style=context>With the arrow keys</>:
<style=key>shift + ←</>: Go south-west
<style=key>↓</>: Go south
<style=key>shift+ →</>: Go south-east
<style=key>←</>: Go west
<style=key>→</>: Go east
<style=key>ctrl + ←</>: Go north-west
<style=key>↑</>: Go north
<style=key>ctrl+ →</>: Go north-east
<style=context>With the numpad keys</>:
<style=key>1</>: Go south-west
<style=key>2</>: Go south
<style=key>3</>: Go south-east
<style=key>4</>: Go west
<style=key>5</>: Idle
<style=key>6</>: Go east
<style=key>7</>: Go north-west
<style=key>8</>: Go north
<style=key>9</>: Go north-east
)help";

    constexpr gf::RectI ActionHelpBox = gf::RectI::from_position_size({ 1, 22 }, { 47, 31 });

    constexpr std::string_view ActionHelpText = R"help(
<style=key>M</>: Mount/Dismount an animal
<style=key>R</>: Reload a weapon
)help";

    constexpr gf::RectI GeneralHelpBox = gf::RectI::from_position_size({ 48, 1 }, { 47, 52 });

    constexpr std::string_view GeneralHelpText = R"help(
<style=key>F</>: Toggle fullscreen
<style=key>H</>: Show Help
<style=key>Tab</>: Show minimap
<style=key>Space</>: Select an option
<style=key>Escape</>: Quit the game
<style=context>In help mode</>:
<style=key>H</>: Quit help
<style=context>In minimap mode</>:
<style=key>Tab</>: Quit minimap
<style=key>+</> or <style=key>F11</>: Zoom in
<style=key>-</> or <style=key>F12</>: Zoom out
)help";

    gf::Console compute_help_console()
    {
      gf::ConsoleStyle style;
      style.color.foreground = gf::White;
      style.color.background = RpgBlue;
      style.effect = gf::ConsoleEffect::set();

      gf::ConsoleRichStyle rich_style;
      rich_style.set_default_style(style);
      rich_style.set_style("key", { gf::Cerise, RpgBlue });
      rich_style.set_style("context", { gf::gray(0.7f), RpgBlue });

      gf::Console console(ConsoleSize);
      gf::console_clear(console, style);
      gf::console_draw_frame(console, gf::RectI::from_size(ConsoleSize), style);

      gf::console_draw_frame(console, MoveHelpBox, style, gf::ConsoleMode::Picture, "Move");
      gf::console_print_text(console, MoveHelpBox.shrink_by(1), gf::ConsoleAlignment::Left, rich_style, MoveHelpText);

      gf::console_draw_frame(console, ActionHelpBox, style, gf::ConsoleMode::Picture, "Action");
      gf::console_print_text(console, ActionHelpBox.shrink_by(1), gf::ConsoleAlignment::Left, rich_style, ActionHelpText);

      gf::console_draw_frame(console, GeneralHelpBox, style, gf::ConsoleMode::Picture, "General");
      gf::console_print_text(console, GeneralHelpBox.shrink_by(1), gf::ConsoleAlignment::Left, rich_style, GeneralHelpText);

      return console;
    }


  }

  AdventureHelpScene::AdventureHelpScene(FarWest* game)
  : m_game(game)
  , m_action_group(compute_settings())
  , m_console(compute_help_console())
  {
  }

  void AdventureHelpScene::process_event(const gf::Event& event)
  {
    m_action_group.process_event(event);
  }

  void AdventureHelpScene::handle_actions()
  {
    using namespace gf::literals;

    if (m_action_group.active("back"_id)) {
      m_game->replace_scene(&m_game->adventure_control);
    }

    m_action_group.reset();
  }

  void AdventureHelpScene::render(gf::Console& console)
  {
    gf::console_blit_to(m_console, console, { 0, 0 }, 1.0f, RpgBlueAlpha);
  }

  gf::ActionGroupSettings AdventureHelpScene::compute_settings()
  {
    using namespace gf::literals;
    gf::ActionGroupSettings settings;

    settings.actions.emplace("back"_id, gf::instantaneous_action().add_keycode_control(gf::Keycode::H));

    return settings;
  }

}
