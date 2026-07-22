#ifndef FW_CONTEXTUAL_CONSOLE_ENTITY_H
#define FW_CONTEXTUAL_CONSOLE_ENTITY_H

#include <gf2/core/ConsoleEntity.h>

#include "Date.h"

namespace fw {
  class FarWest;

  class ContextualConsoleEntity : public gf::ConsoleEntity {
  public:
    ContextualConsoleEntity(FarWest* game);

    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
    Date m_latest_update = {};

    struct Element {
      char16_t picture;
      std::string name;
      gf::Color foreground;
      gf::Color background;
      gf::Vec2I position;
      int32_t distance;
    };

    std::vector<Element> m_actors;

  };

}

#endif // FW_CONTEXTUAL_CONSOLE_ENTITY_H
