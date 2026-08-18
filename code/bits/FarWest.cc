#include "FarWest.h"

#include <filesystem>

#include <fmt/std.h>

#include <gf2/core/Clock.h>
#include <gf2/core/Log.h>
#include <gf2/core/Time.h>

#include "FarWestScene.h"
#include "Settings.h"
#include "WorldGeneration.h"
#include "WorldGenerationStep.h"

namespace fw {

  FarWest::FarWest(FarWestScene* enclosing_scene, gf::Random* random, const std::filesystem::path& datafile, const std::filesystem::path& savefile)
  : gf::ConsoleSceneManager(ConsoleSize)
  , kickoff_title(this)
  , kickoff_menu(this)
  , kickoff_creation(this)
  , adventure_primary(this)
  , adventure_control(this)
  , adventure_inventory(this)
  , adventure_minimap(this)
  , adventure_help(this)
  , adventure_quit(this)
  , adventure_save(this)
  , m_enclosing_scene(enclosing_scene)
  , m_random(random)
  , m_datafile(datafile)
  , m_savefile(savefile)
  , m_model(random)
  {
    push_scene(&kickoff_title);
    push_scene(&kickoff_menu);
  }

  void FarWest::create_world(AdventureChoice choice)
  {
    m_async_world_finished = false;
    m_analysis.set_step(WorldGenerationStep::Start);

    m_async_world = std::async(std::launch::async, [&,choice]() {
      m_analysis.set_step(WorldGenerationStep::File);
      m_model.data.load_from_file(m_datafile);

      if (choice == AdventureChoice::New) {
        if (has_save()) {
          // remove previous savefile
          std::filesystem::remove(m_savefile);
        }

        m_model.state = generate_world(m_random, m_model.data, m_analysis);
      } else {
        assert(has_save());
        gf::Clock clock;
        m_analysis.set_step(WorldGenerationStep::Load);
        m_model.state.load_from_file(m_savefile);
        gf::Log::info("Game loaded in {:g}s from file {}", clock.elapsed_time().as_seconds(), m_savefile);
        std::filesystem::remove(m_savefile);
      }

      m_model.bind(m_analysis);

      if (choice == AdventureChoice::New) {
        m_analysis.set_step(WorldGenerationStep::FirstTurn);
        m_model.update(gf::milliseconds(16));
      }

      m_analysis.set_step(WorldGenerationStep::End);
      m_analysis.print_analysis();
    });
  }

  bool FarWest::world_creation_finished()
  {
    if (m_async_world.valid() && m_async_world.wait_for(std::chrono::seconds::zero()) == std::future_status::ready) {
      m_async_world.get();
      m_async_world_finished = true;
    }

    return m_async_world_finished;
  }

  WorldGenerationStep FarWest::world_creation_step()
  {
    return m_analysis.step();
  }

  void FarWest::start_adventure()
  {
    pop_all_scenes();
    push_scene(&adventure_primary);
    push_scene(&adventure_control);
  }

  bool FarWest::has_save() const
  {
    return std::filesystem::is_regular_file(m_savefile);
  }

  void FarWest::create_save()
  {
    m_async_save_finished = false;

    m_async_save = std::async(std::launch::async, [&]() {
      gf::Clock clock;
      m_model.state.save_to_file(m_savefile);
      gf::Log::info("Game saved in {:g}s to file {}", clock.elapsed_time().as_seconds(), m_savefile);
    });
  }

  bool FarWest::save_creation_finished()
  {
    if (m_async_save.valid() && m_async_save.wait_for(std::chrono::seconds::zero()) == std::future_status::ready) {
      m_async_save.get();
      m_async_save_finished = true;
    }

    return m_async_save_finished;
  }

  gf::Vec2I FarWest::point_to(gf::Vec2F mouse)
  {
    const gf::Vec2F location = m_enclosing_scene->position_to_world_location(mouse);

    gf::OrthogonalGrid grid(ConsoleSize, { 64, 64 }); // TODO: magic constant
    return grid.compute_position(location);
  }

}
