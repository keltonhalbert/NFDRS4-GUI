#include <NFDRSGUI/NFDRSGUI.h>

#include <cstddef>

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

void MainApp::RenderLoop() {
    // Main loop
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    static bool show_imgui_demo = false;
    static bool show_helpmarkers = false;

    ImGuiID dockspace_id, dock_main_id, dock_id_hodo, dock_id_vert_1,
        dock_id_vert_2, dock_id_small_1, dock_id_small_2, dock_id_small_3,
        dock_id_small_4, dock_id_bottom_1, dock_id_bottom_2, dock_id_bottom_3,
        dock_id_bottom_4;

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
        if (glfwGetWindowAttrib(m_main_window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
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
                ImGui::MenuItem("Show Help Markers", nullptr,
                                &show_helpmarkers);
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
            ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.75f,
                                        &dock_main_id, &dock_id_bottom_1);
            // Split the upper half into the SkewT and Hodograph portions
            ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.6f,
                                        &dock_main_id, &dock_id_hodo);
            // Split the SkewT so tha tthe two vertical plots sit next to
            // it on the right...
            ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.85f,
                                        &dock_main_id, &dock_id_vert_1);
            // Split up the new vertical plot into two...
            ImGui::DockBuilderSplitNode(dock_id_vert_1, ImGuiDir_Left, 0.5f,
                                        &dock_id_vert_1, &dock_id_vert_2);
            // Now split the hodograph into the main hodograph window and
            // the small inset bar
            ImGui::DockBuilderSplitNode(dock_id_hodo, ImGuiDir_Up, 0.85f,
                                        &dock_id_hodo, &dock_id_small_1);
            // Now split the small inset bar into 4 equal pieces
            ImGui::DockBuilderSplitNode(dock_id_small_1, ImGuiDir_Left, 0.5f,
                                        &dock_id_small_1, &dock_id_small_4);
            ImGui::DockBuilderSplitNode(dock_id_small_1, ImGuiDir_Left, 0.5f,
                                        &dock_id_small_1, &dock_id_small_2);
            ImGui::DockBuilderSplitNode(dock_id_small_4, ImGuiDir_Right, 0.5f,
                                        &dock_id_small_4, &dock_id_small_3);
            // Finally, split the bottom inset bar into 4 pieces
            ImGui::DockBuilderSplitNode(dock_id_bottom_1, ImGuiDir_Left, 0.5f,
                                        &dock_id_bottom_1, &dock_id_bottom_4);
            ImGui::DockBuilderSplitNode(dock_id_bottom_1, ImGuiDir_Left, 0.5f,
                                        &dock_id_bottom_1, &dock_id_bottom_2);
            ImGui::DockBuilderSplitNode(dock_id_bottom_4, ImGuiDir_Right, 0.5f,
                                        &dock_id_bottom_4, &dock_id_bottom_3);

            ImGui::DockBuilderDockWindow("SkewT", dock_main_id);
            ImGui::DockBuilderDockWindow("Insets Bar", dock_id_bottom_1);
            ImGui::DockBuilderDockWindow("Hodo", dock_id_hodo);
            ImGui::DockBuilderDockWindow("Vert1", dock_id_vert_1);
            ImGui::DockBuilderDockWindow("Vert2", dock_id_vert_2);
            ImGui::DockBuilderDockWindow("Small1", dock_id_small_1);
            ImGui::DockBuilderDockWindow("Small2", dock_id_small_2);
            ImGui::DockBuilderDockWindow("Small3", dock_id_small_3);
            ImGui::DockBuilderDockWindow("Small4", dock_id_small_4);
            ImGui::DockBuilderDockWindow("Bottom1", dock_id_bottom_1);
            ImGui::DockBuilderDockWindow("Bottom2", dock_id_bottom_2);
            ImGui::DockBuilderDockWindow("Bottom3", dock_id_bottom_3);
            ImGui::DockBuilderDockWindow("Bottom4", dock_id_bottom_4);

            ImGui::DockBuilderFinish(dockspace_id);
        }
        if (show_imgui_demo) {
            ImGui::ShowDemoWindow();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowDockID(dock_main_id, ImGuiCond_Once);
        if (ImGui::Begin("SkewT", nullptr, m_window_flags)) {
        }
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowDockID(dock_id_hodo, ImGuiCond_Once);
        if (ImGui::Begin("Hodo", nullptr, m_window_flags)) {
        }
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowDockID(dock_id_vert_1, ImGuiCond_Once);
        if (ImGui::Begin("WSPD v PRES", nullptr, m_window_flags)) {
        }
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::SetNextWindowDockID(dock_id_vert_2, ImGuiCond_Once);
        if (ImGui::Begin("Vert2", nullptr, m_window_flags)) {
        }
        ImGui::End();

        ImGui::SetNextWindowDockID(dock_id_small_1, ImGuiCond_Once);
        if (ImGui::Begin("Small1", nullptr, m_window_flags)) {
        }
        ImGui::End();

        ImGui::SetNextWindowDockID(dock_id_small_2, ImGuiCond_Once);
        if (ImGui::Begin("Small2", nullptr, m_window_flags)) {
        }
        ImGui::End();

        ImGui::SetNextWindowDockID(dock_id_small_3, ImGuiCond_Once);
        if (ImGui::Begin("Small3", nullptr, m_window_flags)) {
        }
        ImGui::End();

        ImGui::SetNextWindowDockID(dock_id_small_4, ImGuiCond_Once);
        if (ImGui::Begin("Small4", nullptr, m_window_flags)) {
        }
        ImGui::End();

        ImGui::SetNextWindowDockID(dock_id_bottom_1, ImGuiCond_Once);
        if (ImGui::Begin("Bottom1", nullptr, m_window_flags)) {
        }
        ImGui::End();

        ImGui::SetNextWindowDockID(dock_id_bottom_2, ImGuiCond_Once);
        if (ImGui::Begin("Bottom2", nullptr, m_window_flags)) {
        }
        ImGui::End();

        ImGui::SetNextWindowDockID(dock_id_bottom_3, ImGuiCond_Once);
        if (ImGui::Begin("Bottom3", nullptr, m_window_flags)) {
        }
        ImGui::End();

        ImGui::SetNextWindowDockID(dock_id_bottom_4, ImGuiCond_Once);
        if (ImGui::Begin("Bottom4", nullptr, m_window_flags)) {
        }
        ImGui::End();

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

        glfwSwapBuffers(m_main_window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif
}  // namespace nfdrs

}  // namespace nfdrs
