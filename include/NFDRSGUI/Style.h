
#include <imgui.h>

namespace nfdrs {
inline void Style() {
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;

    // Base color scheme
    colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.11f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.26f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.15f, 0.15f, 0.16f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.20f, 0.20f, 0.21f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.21f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.28f, 0.28f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.33f, 0.34f, 0.35f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.40f, 0.41f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.412, 0.188, 0.188, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.635, 0.192, 0.192, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.35f, 0.40f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.30f, 0.35f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.25f, 0.80f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.30f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.35f, 0.35f, 0.80f);
    colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.545, 0.545, 0.545, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(1.0f, 1.0f, 1.00f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.502, 0.49, 0.49, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.757, 0.757, 0.761, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.91, 0.914, 0.918, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_TabSelected] = ImVec4(0.412, 0.188, 0.188, 1.0);
    colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.412, 0.187, 0.188, 1.0);
    colors[ImGuiCol_TabHovered] = ImVec4(0.38, 0.38, 0.384, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.412, 0.188, 0.188, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.557, 0.561, 0.561, 1.00f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(1, 0.867, 0.325, 0.678);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.28f, 0.56f, 1.00f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.28f, 0.56f, 1.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    // Style adjustments
    style.WindowRounding = 5.3f;
    style.FrameRounding = 2.3f;
    style.ScrollbarRounding = 0;

    style.WindowTitleAlign = ImVec2(0.50f, 0.50f);
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(5.0f, 5.0f);
    style.ItemSpacing = ImVec2(6.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 6.0f);
    style.IndentSpacing = 25.0f;
}

}  // namespace nfdrs
