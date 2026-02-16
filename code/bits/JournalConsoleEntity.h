#ifndef FW_JOURNAL_CONSOLE_ENTITY_H
#define FW_JOURNAL_CONSOLE_ENTITY_H

#include <gf2/core/Time.h>
#include <gf2/core/ConsoleEntity.h>

namespace fw {
  class FarWest;

  class JournalConsoleEntity : public gf::ConsoleEntity {
  public:
    JournalConsoleEntity(FarWest* game);

    void update(gf::Time time) override;
    void render(gf::Console& console) override;

  private:
    FarWest* m_game = nullptr;
  };

}

#endif // FW_JOURNAL_CONSOLE_ENTITY_H
