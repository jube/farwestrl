#ifndef FW_MESSAGE_LOG_ELEMENT_H
#define FW_MESSAGE_LOG_ELEMENT_H

#include <gf2/core/Time.h>
#include <gf2/core/ConsoleElement.h>

namespace fw {
  class FarWest;

  class MessageLogElement : public gf::ConsoleElement {
  public:
    MessageLogElement(FarWest* game);

    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
  };

}

#endif // FW_MESSAGE_LOG_ELEMENT_H
