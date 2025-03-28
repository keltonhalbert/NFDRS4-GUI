
#include <NFDRSGUI/NFDRSGUI.h>

#include "NFDRSGUI/FW21Decoder.h"
#include "NFDRSGUI/ModelRunners.h"
#include "imgui.h"

namespace nfdrs {

static void individual_settings(const char* title, DeadFuelModelRunner& dfm,
                                fw21::FW21Timeseries& data) {
    if (ImGui::BeginTabItem(title)) {
        ImGui::PushItemWidth(ImGui::GetFontSize() * -15);
        ImGui::InputInt("Random Seed", &dfm.settings.random_seed);
        ImGui::InputInt("Diffusivity Steps", &dfm.settings.diffusivity_steps);
        ImGui::InputInt("Moisture Steps", &dfm.settings.moisture_steps);
        ImGui::InputInt("Stick Nodes", &dfm.settings.stick_nodes);
        ImGui::InputDouble("Adsorption Rate", &dfm.settings.adsorption_rate);
        ImGui::InputDouble("Desorption Rate", &dfm.settings.desorption_rate);
        ImGui::InputDouble("Planar Heat Transfer Rate",
                           &dfm.settings.planar_heat_transfer_rate);
        ImGui::InputDouble("Stick Length", &dfm.settings.stick_length);
        ImGui::SliderFloat("Stick Density", &dfm.settings.stick_density, 0.001f,
                           1.0f, "%.3f");
        ImGui::SliderFloat("Maximum Local Moisture",
                           &dfm.settings.max_local_moisture, 0.0f, 1.0f,
                           "%.3f");
        if (ImGui::Button("Default")) {
            dfm.default_settings();
        }
        ImGui::SameLine();
        if (ImGui::Button("Run")) {
            if (dfm.model->updates() > 0) {
                dfm.reset();
            }
            dfm.run(data);
        }
        ImGui::SameLine();
        ImGui::ProgressBar(dfm.progress);
        ImGui::PopItemWidth();
        ImGui::EndTabItem();
    }
}

void dead_fuel_settings(bool& enabled, DeadFuelModelRunner& dfm_1h,
                        DeadFuelModelRunner& dfm_10h,
                        DeadFuelModelRunner& dfm_100h,
                        DeadFuelModelRunner& dfm_1000h,
                        fw21::FW21Timeseries& data) {
    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(
        ImVec2(main_viewport->WorkPos.x + 100, main_viewport->WorkPos.y + 20),
        ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Dead Fuel Model Settings", &enabled)) {
        if (ImGui::BeginTabBar("Dead Fuel Models")) {
            individual_settings("1h Fuels", dfm_1h, data);
            individual_settings("10h Fuels", dfm_10h, data);
            individual_settings("100h Fuels", dfm_100h, data);
            individual_settings("1000h Fuels", dfm_1000h, data);
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

}  // namespace nfdrs
