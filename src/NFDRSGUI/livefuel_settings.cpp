
#include <NFDRSGUI/NFDRSGUI.h>

#include "NFDRSGUI/ModelRunners.h"
#include "imgui.h"

namespace nfdrs {

static void individual_settings(const char* title) {
    if (ImGui::BeginTabItem(title)) {
        if (ImGui::Button("Default")) {
        }
        ImGui::SameLine();
        if (ImGui::Button("Run")) {
        }
        ImGui::SameLine();
        ImGui::ProgressBar(0);
        ImGui::EndTabItem();
    }
}

void live_fuel_settings(bool& enabled) {
    if (ImGui::Begin("Live Fuel Model Settings", &enabled)) {
        if (ImGui::BeginTabBar("Live Fuel Models")) {
            individual_settings("Herbaceous Fuels");
            individual_settings("Woody Fuels");
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

}  // namespace nfdrs
