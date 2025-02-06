
#include <NFDRSGUI/NFDRSGUI.h>

namespace nfdrs {

static void individual_settings(const char* title, DeadFuelModelRunner& dfm,
                                Meteogram& data) {
    if (ImGui::BeginTabItem(title)) {
        if (ImGui::Button("Run")) {
            if (dfm.model->updates() > 0) {
                dfm.reset();
            }
            dfm.run(data);
        }
        ImGui::EndTabItem();
    }
}

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
