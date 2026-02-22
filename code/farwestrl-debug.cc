// SPDX-License-Identifier: Zlib
// Copyright (c) 2023-2025 Julien Bernard
#include <cstdlib>

#include <imgui.h>

#include <gf2/graphics/GamePaths.h>
#include <gf2/graphics/Scene.h>
#include <gf2/graphics/SceneManager.h>
#include <gf2/imgui/ImguiEntity.h>
#include <gf2/imgui/ImguiInitializer.h>
#include <gf2/imgui/ImguiManager.h>

#include "bits/WorldState.h"

#include "config-debug.h"

namespace {

  const char* to_string(fw::LocalityType type)
  {
    switch (type) {
      case fw::LocalityType::Farm:
        return "Farm";
      case fw::LocalityType::Camp:
        return "Camp";
      case fw::LocalityType::Village:
        return "Village";
    }

    return "??";
  }


  constexpr ImGuiWindowFlags DefaultWindowFlags = ImGuiWindowFlags_NoSavedSettings;

  class ImguiScene : public gf::Scene {
  public:
    ImguiScene(gf::BasicSceneManager* scene_manager, const std::filesystem::path& state_file)
    : m_manager(scene_manager->window(), scene_manager->render_manager())
    , m_state_file(state_file)
    {
      add_hud_entity(&m_entity);
      load_state();
    }

  private:

    bool do_early_process_event(const gf::Event& event) override
    {
      return m_manager.process_event(event);
    }

    void do_update(gf::Time time) override
    {
      m_manager.update(time);

      ImGui::NewFrame();
      update_entities(time);

      // ImGui::ShowDemoWindow(&m_show_demo);
      main_window();

      ImGui::EndFrame();
      ImGui::Render();

      m_entity.set_draw_data(ImGui::GetDrawData());
    }

    void main_window()
    {
      if (ImGui::Begin("Far West RL Editor", nullptr, DefaultWindowFlags)) {
        if (ImGui::BeginTabBar("##Tabs")) {

          if (ImGui::BeginTabItem("Map")) {
            map();
            ImGui::EndTabItem();
          }

          if (ImGui::BeginTabItem("Actors")) {
            ImGui::EndTabItem();
          }

          if (ImGui::BeginTabItem("Items")) {
            ImGui::EndTabItem();
          }

          if (ImGui::BeginTabItem("Scheduler")) {
            ImGui::EndTabItem();
          }

          if (ImGui::BeginTabItem("Journal")) {
            ImGui::EndTabItem();
          }

          if (ImGui::BeginTabItem("Misc")) {
            misc();
            ImGui::EndTabItem();
          }

          ImGui::EndTabBar();
        }

        ImGui::Spacing();
        ImGui::Separator();
        // ImGui::Text("%s", m_status_text.c_str()); // NOLINT(cppcoreguidelines-pro-type-vararg)
      }

      ImGui::End();
    }

    void map()
    {
      if (ImGui::TreeNode("Towns")) {
        if (ImGui::BeginTable("##TownTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {

          ImGui::TableSetupColumn("Index");
          ImGui::TableSetupColumn("Position");
          ImGui::TableSetupColumn("Horizontal Street");
          ImGui::TableSetupColumn("Vertical Street");
          ImGui::TableHeadersRow();

          for (auto [index, town] : gf::enumerate(m_state.map.towns)) {
            ImGui::TableNextColumn();
            ImGui::Text("%zu", index); // NOLINT(cppcoreguidelines-pro-type-vararg)

            ImGui::TableNextColumn();
            ImGui::Text("%i, %i", town.position.x, town.position.y); // NOLINT(cppcoreguidelines-pro-type-vararg)

            ImGui::TableNextColumn();
            ImGui::Text("%i", town.horizontal_street); // NOLINT(cppcoreguidelines-pro-type-vararg)

            ImGui::TableNextColumn();
            ImGui::Text("%i", town.vertical_street); // NOLINT(cppcoreguidelines-pro-type-vararg)
          }

          ImGui::EndTable();
        }

        ImGui::TreePop();
      }

      if (ImGui::TreeNode("Localities")) {
        if (ImGui::BeginTable("##LocalitiesTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
          ImGui::TableSetupColumn("Index");
          ImGui::TableSetupColumn("Position");
          ImGui::TableSetupColumn("Type");
          ImGui::TableHeadersRow();

          for (auto [index, locality] : gf::enumerate(m_state.map.localities)) {
            ImGui::TableNextColumn();
            ImGui::Text("%.02zu", index); // NOLINT(cppcoreguidelines-pro-type-vararg)

            ImGui::TableNextColumn();
            ImGui::Text("%i, %i", locality.position.x, locality.position.y); // NOLINT(cppcoreguidelines-pro-type-vararg)

            ImGui::TableNextColumn();
            ImGui::Text("%s", to_string(locality.type)); // NOLINT(cppcoreguidelines-pro-type-vararg)
          }

          ImGui::EndTable();
        }

        ImGui::TreePop();
      }
    }

    void misc()
    {
      if (ImGui::BeginTable("##Misc", 2)) {

        ImGui::TableNextColumn();
        ImGui::TableSetupColumn("Property");

        ImGui::TableNextColumn();
        ImGui::TableSetupColumn("Value");


        ImGui::TableNextColumn();
        ImGui::Text("%s", "Date"); // NOLINT(cppcoreguidelines-pro-type-vararg)

        ImGui::TableNextColumn();
        const std::string date = m_state.current_date.to_string();
        ImGui::Text("%s", date.c_str()); // NOLINT(cppcoreguidelines-pro-type-vararg)

        ImGui::EndTable();
      }

    }


    void load_state()
    {
      // TODO: async load
      gf::Log::debug("Start loading state...");
      m_state.load_from_file(m_state_file);
      gf::Log::debug("Finish loading state...");
    }

    gf::ImguiManager m_manager;
    gf::ImguiEntity m_entity;

    std::filesystem::path m_state_file;
    fw::WorldState m_state;

    bool m_show_demo = true;
  };

}

int main()
{
  const std::filesystem::path assets_directory = fw::FarWestDebugDataDirectory;
  const std::filesystem::path font_file = assets_directory / "DejaVuSans.ttf";
  const std::filesystem::path state_file = gf::user_data_path("jube", "farfarwest") / "save.dat";

  gf::SingleSceneManager scene_manager("Far West RL | Debug", gf::vec(1600, 900));
  const gf::ImguiInitializer imgui_initializer;

  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.IniFilename = nullptr;
  io.Fonts->AddFontFromFileTTF(font_file.string().c_str(), 16.0f);

  ImguiScene scene(&scene_manager, state_file);
  return scene_manager.run(&scene);
}
