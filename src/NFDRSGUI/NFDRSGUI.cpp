#include <NFDRSGUI/Meteogram.h>
#include <NFDRSGUI/ModelRunners.h>
#include <NFDRSGUI/NFDRSGUI.h>
#include <NFDRSGUI/data.h>
#include <deadfuelmoisture.h>
#include <nfdrs4.h>

#include <atomic>
#include <ctime>
#include <memory>
#include <thread>

#include "imgui.h"
#include "imgui_internal.h"

namespace nfdrs {

double ClockSeconds() {
    static ClockSeconds_ watch;
    return watch.elapsed();
}

void IdleBySleeping(FPSIdling& idling) {
    idling.is_idling = false;
    if ((idling.fps_idle > 0.f) && (idling.idling_enabled)) {
        double before_wait = ClockSeconds();
        double wait_timeout = 1. / (double)idling.fps_idle;

        glfwWaitEventsTimeout(wait_timeout);
        double after_wait = ClockSeconds();
        double wait_duration = after_wait - before_wait;
        double wait_idle_expected = 1. / (double)idling.fps_idle;
        idling.is_idling = wait_duration > wait_idle_expected * 0.9;
    }
}

// Logic for idling under emscripten
// This test should be done after calling Impl_PollEvents() since it checks the
// event queue for incoming events!
bool ShallIdleThisFrame_Emscripten(FPSIdling& idling) {
    ImGuiContext& g = *GImGui;
    bool hasInputEvent = !g.InputEventsQueue.empty();

    if (!idling.idling_enabled) {
        idling.is_idling = false;
        return false;
    }

    static double lastRefreshTime = 0.;
    double now = ClockSeconds();

    bool shallIdleThisFrame = false;
    if (hasInputEvent) {
        idling.is_idling = false;
        shallIdleThisFrame = false;
    } else {
        idling.is_idling = true;
        if ((now - lastRefreshTime) < 1. / idling.fps_idle)
            shallIdleThisFrame = true;
        else
            shallIdleThisFrame = false;
    }

    if (!shallIdleThisFrame) lastRefreshTime = now;

    return shallIdleThisFrame;
}

int spc_fire_cat(double tair, double relh, double wspd) {
    // Elevated -- 15 MPH
    int spc_cat = 0;
    if ((wspd >= 6.7056) && (relh <= 25) && (tair >= 7.2222)) {
        spc_cat = 1;
    }
    if ((wspd >= 8.9408) && (relh <= 20) && (tair >= 10)) {
        spc_cat = 2;
    }
    if ((wspd >= 13.4112) && (relh <= 15) && (tair >= 15.5556)) {
        spc_cat = 3;
    }

    return spc_cat;
}

void firewx_category(Meteogram& met_data) {
    for (std::ptrdiff_t idx = 0; idx < met_data.N; ++idx) {
        met_data.m_firewx_cat[idx] = spc_fire_cat(
            met_data.m_tair[idx], met_data.m_relh[idx], met_data.m_wspd[idx]);
    }
}

void MainApp::RenderLoop() {
    // Main loop
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    static bool show_imgui_demo = false;
    static bool show_helpmarkers = false;

    static Meteogram met_data(timestamp, relh, tmpc, wspd, wdir, gust, rain,
                              pres, srad, NSTATIC);
    firewx_category(met_data);

    // Dead Fuel Moisture models
    DeadFuelModelRunner dfm_1hour =
        DeadFuelModelRunner(0.20, "1-hour", met_data);
    DeadFuelModelRunner dfm_10hour =
        DeadFuelModelRunner(0.64, "10-hour", met_data);
    DeadFuelModelRunner dfm_100hour =
        DeadFuelModelRunner(2.0, "100-hour", met_data);
    DeadFuelModelRunner dfm_1000hour =
        DeadFuelModelRunner(6.40, "1000-hour", met_data);

    /*dfm_1hour.run(met_data);*/
    /*dfm_10hour.run(met_data);*/
    /*dfm_100hour.run(met_data);*/
    /*dfm_1000hour.run(met_data);*/

    ImGuiID dockspace_id, dock_main_id;

#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    // enter the render loop
    while (!glfwWindowShouldClose(m_main_window))
#endif
    {
#ifndef __EMSCRIPTEN__
        IdleBySleeping(m_fps_idling);
#endif

        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
        // tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data
        // to your main application, or clear/overwrite your copy of the mouse
        // data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
        // data to your main application, or clear/overwrite your copy of the
        // keyboard data. Generally you may always pass all inputs to dear
        // imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

#ifdef __EMSCRIPTEN__
        if (ShallIdleThisFrame_Emscripten(m_fps_idling)) return;
#endif

        // Sleep if the window is minimized - don't do any unnecessary
        // computation or drawing!
#ifndef __EMSCRIPTEN__
        if (glfwGetWindowAttrib(m_main_window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
#endif

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        dockspace_id = ImGui::GetID("NFDRSGUI-Dockspace");

        ImGui::DockSpaceOverViewport(dockspace_id, m_main_viewport,
                                     m_dockspace_flags);

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Menu")) {
                ImGui::MenuItem("DemoWindow", nullptr, &show_imgui_demo);
                ImGui::MenuItem("Show Help Markers", nullptr,
                                &show_helpmarkers);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Configure & Run")) {
                ImGui::MenuItem("Dead Fuel Moisture Model", nullptr,
                                &show_dead_fuel_settings);
                ImGui::MenuItem("Live Fuel Moisture Model", nullptr);
                ImGui::MenuItem("NFDRS4 Model", nullptr);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (m_dock_init) {
            m_dock_init = false;
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, m_dockspace_flags);

            dock_main_id = dockspace_id;
            // split the main window into the top and bottom portions of the
            // frame, with the SkewT and Hodograph in the top half and the
            // bottom inset bar in the lower half
            /*ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.80f,*/
            /*                            &dock_main_id, &dock_id_bottom_1);*/
            /**/
            /*ImGui::DockBuilderDockWindow("Meteograms", dock_main_id);*/
            /*ImGui::DockBuilderDockWindow("Model Configuration Bar",*/
            /*                             dock_id_bottom_1);*/

            ImGui::DockBuilderFinish(dockspace_id);
        }
        if (show_imgui_demo) {
            ImGui::ShowDemoWindow();
        }

        /*ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f,
         * 0.0f));*/
        ImGui::SetNextWindowDockID(dock_main_id, ImGuiCond_Once);
        if (ImGui::Begin("Station Meteogram", nullptr, m_window_flags)) {
            meteogram(met_data.m_timestamp.get(), met_data.m_tair.get(),
                      met_data.m_relh.get(), met_data.m_wspd.get(),
                      met_data.m_wdir.get(), met_data.m_gust.get(),
                      met_data.m_rain.get(), met_data.m_srad.get(),
                      met_data.m_firewx_cat.get(),
                      dfm_1hour.radial_moisture.get(),
                      dfm_10hour.radial_moisture.get(),
                      dfm_100hour.radial_moisture.get(),
                      dfm_1000hour.radial_moisture.get(),
                      dfm_1hour.fuel_temperature.get(),
                      dfm_10hour.fuel_temperature.get(),
                      dfm_100hour.fuel_temperature.get(),
                      dfm_1000hour.fuel_temperature.get(), met_data.N);
        }
        ImGui::End();
        /*ImGui::PopStyleVar();*/

        if (show_dead_fuel_settings)
            dead_fuel_settings(show_dead_fuel_settings, dfm_1hour, dfm_10hour,
                               dfm_100hour, dfm_1000hour, met_data);

        /*ImGui::SetNextWindowDockID(dock_id_bottom_1, ImGuiCond_Once);*/
        /*if (ImGui::Begin("Model Config", nullptr, m_window_flags)) {*/
        /*    // pass*/
        /*}*/
        /*ImGui::End();*/

        // 1. Show the big demo window (Most of the sample code is in
        // ImGui::ShowDemoWindow()! You can browse its code to learn more about
        // Dear ImGui!).
        // bool show_demo_window = true;
        // if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
        //
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(m_main_window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(m_clear_color.x * m_clear_color.w,
                     m_clear_color.y * m_clear_color.w,
                     m_clear_color.z * m_clear_color.w, m_clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we
        // save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call
        //  glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

#ifndef __EMSCRIPTEN__
        glfwSwapBuffers(m_main_window);
#endif
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif
}  // namespace nfdrs

}  // namespace nfdrs
