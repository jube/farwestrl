#ifndef FW_CONTEXTUAL_CONSOLE_ENTITY_H
#define FW_CONTEXTUAL_CONSOLE_ENTITY_H

#include <gf2/core/ConsoleEntity.h>

namespace fw {
  class FarWest;

  class ContextualConsoleEntity : public gf::ConsoleEntity {
  public:
    ContextualConsoleEntity(FarWest* game);

    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
  };

}

#endif // FW_CONTEXTUAL_CONSOLE_ENTITY_H
