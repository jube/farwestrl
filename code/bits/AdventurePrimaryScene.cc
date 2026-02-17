#include "AdventurePrimaryScene.h"

#include "FarWest.h"

namespace fw {

  AdventurePrimaryScene::AdventurePrimaryScene(FarWest* game)
  : m_game(game)
  , m_message_log_element(game)
  , m_map_element(game)
  , m_hero_element(game)
  , m_contextual_element(game)
  {
    add_model(game->model());

    add_entity(&m_message_log_element);
    add_entity(&m_map_element);
    add_entity(&m_hero_element);
    add_entity(&m_contextual_element);
  }

}
