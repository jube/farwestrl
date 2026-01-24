#ifndef FW_PRIMARY_SCENE_H
#define FW_PRIMARY_SCENE_H

#include <gf2/core/ConsoleScene.h>

#include "ContextualElement.h"
#include "HeroElement.h"
#include "MapElement.h"
#include "MessageLogElement.h"

namespace fw {
  class FarWest;

  class PrimaryScene : public gf::ConsoleScene {
  public:
    PrimaryScene(FarWest* game);

  private:
    FarWest* m_game = nullptr;

    MessageLogElement m_message_log_element;
    MapElement m_map_element;
    HeroElement m_hero_element;
    ContextualElement m_contextual_element;
  };


}

#endif // FW_PRIMARY_SCENE_H
