#include <NFDRSGUI/FW21Decoder.h>
#include <NFDRSGUI/ModelRunners.h>
#include <NFDRSGUI/NFDRSGUI.h>
#include <deadfuelmoisture.h>
#include <nfdrs4.h>

#include <cstddef>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten_browser_file.h>
#endif

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

void parse_uploaded_file(std::string const& filename,
                         std::string const& mime_type, std::string_view buffer,
                         void* callback_data = nullptr) {
    if (!buffer.empty()) {
        fw21::FW21Timeseries decoded =
            fw21::FW21Timeseries::decode_fw21(buffer);

        if (callback_data != nullptr) {
            auto* met_data_ptr =
                static_cast<std::unique_ptr<fw21::FW21Timeseries>*>(
                    callback_data);
            *met_data_ptr =
                std::make_unique<fw21::FW21Timeseries>(std::move(decoded));
        }
    }
}

void MainApp::RenderLoop() {
    // Main loop
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    static bool show_imgui_demo = false;
    static bool show_helpmarkers = false;
    static bool data_are_initialized = false;

    std::unique_ptr<fw21::FW21Timeseries> met_data;

    // Dead Fuel Moisture models
    std::unique_ptr<DeadFuelModelRunner> dfm_1hour;
    std::unique_ptr<DeadFuelModelRunner> dfm_10hour;
    std::unique_ptr<DeadFuelModelRunner> dfm_100hour;
    std::unique_ptr<DeadFuelModelRunner> dfm_1000hour;

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

        // Sleep if the window is minimized - don't do any unnecessary
        // computation or drawing!
        if (glfwGetWindowAttrib(m_main_window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
#endif

        if ((met_data) && (!data_are_initialized)) {
            dfm_1hour = std::make_unique<DeadFuelModelRunner>(0.20, "1-hour",
                                                              *met_data);
            dfm_10hour = std::make_unique<DeadFuelModelRunner>(0.64, "10-hour",
                                                               *met_data);
            dfm_100hour = std::make_unique<DeadFuelModelRunner>(2.0, "100-hour",
                                                                *met_data);
            dfm_1000hour = std::make_unique<DeadFuelModelRunner>(
                6.40, "1000-hour", *met_data);

            data_are_initialized = true;
        }

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
                /*ImGui::MenuItem("Show Help Markers", nullptr,*/
                /*                &show_helpmarkers);*/
                ImGui::MenuItem("Idle FPS", nullptr,
                                &m_fps_idling.idling_enabled);
                ImGui::EndMenu();
            }
#ifdef __EMSCRIPTEN__
            ImGui::MenuItem("Upload Data", nullptr, &show_upload_window);
#endif
            if (ImGui::BeginMenu("Configure & Run")) {
                ImGui::MenuItem("Dead Fuel Moisture Model", nullptr,
                                &show_dead_fuel_settings);
                ImGui::MenuItem("Live Fuel Moisture Model", nullptr,
                                &show_live_fuel_settings);
                ImGui::MenuItem("NFDRS4 Model", nullptr, &show_nfdrs_settings);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (m_dock_init) {
            m_dock_init = false;
            ImGui::DockBuilderRemoveNode(dockspace_id);
            ImGui::DockBuilderAddNode(dockspace_id, m_dockspace_flags);
            dock_main_id = dockspace_id;
            /*ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.80f,*/
            /*                            &dock_main_id, &dock_id_bottom_1);*/
            ImGui::DockBuilderDockWindow("Meteograms", dock_main_id);
            ImGui::DockBuilderFinish(dockspace_id);
        }
        if (show_imgui_demo) {
            ImGui::ShowDemoWindow();
        }

        /*ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f,
         * 0.0f));*/
        ImGui::SetNextWindowDockID(dock_main_id, ImGuiCond_Once);
        if (ImGui::Begin("Station Meteogram", nullptr, 0)) {
            meteogram(met_data, *dfm_1hour, *dfm_10hour, *dfm_100hour,
                      *dfm_1000hour, m_layout_threshold);
        }
        ImGui::End();
        /*ImGui::PopStyleVar();*/

        if (show_dead_fuel_settings)
            dead_fuel_settings(show_dead_fuel_settings, *dfm_1hour, *dfm_10hour,
                               *dfm_100hour, *dfm_1000hour, *met_data);

        if (show_live_fuel_settings)
            live_fuel_settings(show_live_fuel_settings);

        if (show_nfdrs_settings) nfdrs_settings(show_nfdrs_settings);

#ifdef __EMSCRIPTEN__
        if (show_upload_window) {
            emscripten_browser_file::upload(".fw21", parse_uploaded_file,
                                            static_cast<void*>(&met_data));
            show_upload_window = false;
        }
#endif

        // 1. Show the big demo window (Most of the sample code is in
        // ImGui::ShowDemoWindow()! You can browse its code to learn more
        // about Dear ImGui!). bool show_demo_window = true; if
        // (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
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
