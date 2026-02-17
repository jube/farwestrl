#ifndef FW_FAR_FAR_WEST_H
#define FW_FAR_FAR_WEST_H

#include <cstdint>

#include <filesystem>
#include <future>

#include <gf2/core/ConsoleSceneManager.h>
#include <gf2/core/ConsoleStyle.h>
#include <gf2/core/Random.h>

#include "AdventureControlScene.h"
#include "AdventureHelpScene.h"
#include "AdventureMinimapScene.h"
#include "AdventurePrimaryScene.h"
#include "AdventureQuitScene.h"
#include "AdventureSaveScene.h"
#include "KickoffCreationScene.h"
#include "KickoffMenuScene.h"
#include "KickoffTitleScene.h"
#include "WorldModel.h"
#include "WorldGenerationStep.h"

namespace fw {
  class FarWestScene;

  enum class AdventureChoice : uint8_t {
    New,
    Saved,
  };

  class FarWest : public gf::ConsoleSceneManager {
  public:
    FarWest(FarWestScene* enclosing_scene, gf::Random* random, const std::filesystem::path& datafile, const std::filesystem::path& savefile);

    gf::Random* random()
    {
      return m_random;
    }

    WorldModel* model()
    {
      return &m_model;
    }

    WorldState* state()
    {
      return &m_model.state;
    }

    WorldRuntime* runtime()
    {
      return &m_model.runtime;
    }

    const gf::ConsoleRichStyle& style() const
    {
      return m_rich_style;
    }

    void create_world(AdventureChoice choice);
    bool world_creation_finished();
    WorldGenerationStep world_creation_step();

    void start_adventure();

    bool has_save() const;
    void create_save();
    bool save_creation_finished();

    gf::Vec2I point_to(gf::Vec2F mouse);

    KickoffTitleScene kickoff_title;
    KickoffMenuScene kickoff_menu;
    KickoffCreationScene kickoff_creation;

    AdventurePrimaryScene adventure_primary;
    AdventureControlScene adventure_control;
    AdventureMinimapScene adventure_minimap;
    AdventureHelpScene adventure_help;
    AdventureQuitScene adventure_quit;
    AdventureSaveScene adventure_save;

  private:
    FarWestScene* m_enclosing_scene = nullptr;
    gf::Random* m_random = nullptr;
    std::filesystem::path m_datafile;

    std::filesystem::path m_savefile;
    std::future<void> m_async_save;
    bool m_async_save_finished = false;

    WorldModel m_model;
    std::future<void> m_async_world;
    bool m_async_world_finished = false;

    WorldGenerationAnalysis m_analysis;

    gf::ConsoleRichStyle m_rich_style;
  };

}

#endif // FW_FAR_FAR_WEST_H
