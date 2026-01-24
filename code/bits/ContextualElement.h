#ifndef FW_CONTEXTUAL_ELEMENT_H
#define FW_CONTEXTUAL_ELEMENT_H

#include <gf2/core/ConsoleElement.h>

namespace fw {
  class FarWest;

  class ContextualElement : public gf::ConsoleElement {
  public:
    ContextualElement(FarWest* game);

    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
  };

}

#endif // FW_CONTEXTUAL_ELEMENT_H
