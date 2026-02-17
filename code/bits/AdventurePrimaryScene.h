#ifndef FW_ADVENTURE_PRIMARY_SCENE_H
#define FW_ADVENTURE_PRIMARY_SCENE_H

#include <gf2/core/ConsoleScene.h>

#include "ContextualConsoleEntity.h"
#include "HeroConsoleEntity.h"
#include "MapConsoleEntity.h"
#include "JournalConsoleEntity.h"

namespace fw {
  class FarWest;

  class AdventurePrimaryScene : public gf::ConsoleScene {
  public:
    AdventurePrimaryScene(FarWest* game);

  private:
    FarWest* m_game = nullptr;

    JournalConsoleEntity m_message_log_element;
    MapConsoleEntity m_map_element;
    HeroConsoleEntity m_hero_element;
    ContextualConsoleEntity m_contextual_element;
  };


}

#endif // FW_ADVENTURE_PRIMARY_SCENE_H
