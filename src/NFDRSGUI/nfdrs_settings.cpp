
#include <NFDRSGUI/NFDRSGUI.h>

/*#include "NFDRSGUI/ModelRunners.h"*/
#include "imgui.h"

namespace nfdrs {

void nfdrs_settings(bool& enabled) {
    if (ImGui::Begin("NFDRS4 Settings", &enabled)) {
        if (ImGui::Button("Default")) {
        }
        ImGui::SameLine();
        if (ImGui::Button("Run")) {
        }
        ImGui::SameLine();
        ImGui::ProgressBar(0);
    }
    ImGui::End();
}

}  // namespace nfdrs
