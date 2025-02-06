
#include <NFDRSGUI/NFDRSGUI.h>

namespace nfdrs {

void dead_fuel_settings(bool& enabled, DeadFuelModelRunner& dfm_1h,
                        DeadFuelModelRunner& dfm_10h,
                        DeadFuelModelRunner& dfm_100h,
                        DeadFuelModelRunner& dfm_1000h, Meteogram& data) {
    if (ImGui::Begin("Dead Fuel Model Settings", &enabled)) {
        if (ImGui::BeginTabBar("Dead Fuel Models")) {
            if (ImGui::BeginTabItem("All Fuels")) {
                if (ImGui::Button("Run")) {
                    if (dfm_1h.model->updates() > 0) {
                        dfm_1h.reset();
                    }
                    if (dfm_10h.model->updates() > 0) {
                        dfm_10h.reset();
                    }
                    if (dfm_100h.model->updates() > 0) {
                        dfm_100h.reset();
                    }
                    if (dfm_1000h.model->updates() > 0) {
                        dfm_1000h.reset();
                    }
                    dfm_1h.run(data);
                    dfm_10h.run(data);
                    dfm_100h.run(data);
                    dfm_1000h.run(data);
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("1h Fuels")) {
                if (ImGui::Button("Run")) {
                    if (dfm_1h.model->updates() > 0) {
                        dfm_1h.reset();
                    }
                    dfm_1h.run(data);
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("10h Fuels")) {
                if (ImGui::Button("Run")) {
                    if (dfm_10h.model->updates() > 0) {
                        dfm_10h.reset();
                    }
                    dfm_10h.run(data);
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("100h Fuels")) {
                if (ImGui::Button("Run")) {
                    if (dfm_100h.model->updates() > 0) {
                        dfm_100h.reset();
                    }
                    dfm_100h.run(data);
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("1000h Fuels")) {
                if (ImGui::Button("Run")) {
                    if (dfm_1000h.model->updates() > 0) {
                        dfm_1000h.reset();
                    }
                    dfm_1000h.run(data);
                }
                ImGui::EndTabItem();
            }
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

}  // namespace nfdrs
