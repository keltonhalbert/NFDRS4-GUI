#ifndef NFDRSGUI
#define NFDRSGUI

#include <chrono>
#include <cmath>
#include <cstddef>

#ifdef __EMSCRIPTEN__
#include "emscripten_loop.h"
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot_internal.h"
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>  // Will drag system OpenGL headers
#include <NFDRSGUI/ModelRunners.h>

namespace nfdrs {

void meteogram(const double timestamp[], const double tmpc[],
               const double relh[], const double wspd[], const double wdir[],
               const double gust[], const double precip[], const double srad[],
               const int firewx_cat[], const double dfm_1h[],
               const double dfm_10h[], const double dfm_100h[],
               const double dfm_1000h[], const double dft_1h[],
               const double dft_10h[], const double dft_100h[],
               const double dft_1000h[], std::ptrdiff_t N);

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

class ClockSeconds_ {
    // Typical C++ shamanic incantations to get a time in seconds
   private:
    using Clock = std::chrono::high_resolution_clock;
    using second = std::chrono::duration<double, std::ratio<1>>;
    std::chrono::time_point<Clock> mStart;

   public:
    ClockSeconds_() : mStart(Clock::now()) {}

    double elapsed() const {
        return std::chrono::duration_cast<second>(Clock::now() - mStart)
            .count();
    }
};

struct FPSIdling {
    float fps_idle = 1.f;
    bool idling_enabled = true;
    bool is_idling = false;
};

class MainApp {
    FPSIdling m_fps_idling;
    GLFWwindow* m_main_window;
    ImGuiViewport* m_main_viewport;
    ImGuiWindowFlags m_window_flags;
    ImGuiDockNodeFlags m_dockspace_flags;
    ImGuiStyle m_style;
    ImVec4 m_clear_color;
    bool m_dock_init = true;

    bool show_dead_fuel_settings = false;

   public:
    MainApp() {
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit()) {
            return;
        }

        // Decide GL+GLSL versions
        //
#if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
        const char* glsl_version = "#version 100";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
        // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE,
                       GLFW_OPENGL_CORE_PROFILE);             // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Required on Mac
#else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // // 3.2+ only glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        // // 3.0+ only
#endif

        // Create window with graphics context
        m_main_window = glfwCreateWindow(
            1400, 1000, "NFDRSGUI: Fuel Models Browser for FireWx", nullptr,
            nullptr);
        if (m_main_window == nullptr) return;
        glfwMakeContextCurrent(m_main_window);
        glfwSwapInterval(1);  // Enable vsync

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImPlot::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |=
            ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        io.ConfigFlags |=
            ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
        io.ConfigFlags |=
            ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport /
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
        // Platform Windows
        // io.ConfigViewportsNoAutoMerge = true;
        // io.ConfigViewportsNoTaskBarIcon = true;

        // set fullscreen window attributes
        m_window_flags = ImGuiWindowFlags_NoDecoration |
                         // ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoSavedSettings;
        m_dockspace_flags =
            ImGuiDockNodeFlags_HiddenTabBar | ImGuiDockNodeFlags_NoTabBar;

        // get the viewport so we can fill it with our window
        m_main_viewport = ImGui::GetMainViewport();

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        // ImGui::StyleColorsLight();

        // When viewports are enabled we tweak WindowRounding/WindowBg so
        // platform windows can look identical to regular ones.
        m_style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            m_style.WindowRounding = 0.0f;
            m_style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(m_main_window, true);
#ifdef __EMSCRIPTEN__
        ImGui_ImplGlfw_InstallEmscriptenCallbacks(m_main_window, "#canvas");
#endif
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You
        // can also load multiple fonts and use ImGui::PushFont()/PopFont() to
        // select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if
        // you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return a nullptr.
        // Please handle those errors in your application (e.g. use an
        // assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and
        // stored into a texture when calling
        // ImFontAtlas::Build()/GetTexDataAsXXXX(), which
        // ImGui_ImplXXXX_NewFrame below will call.
        // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use
        // Freetype for higher quality font rendering.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a
        // string literal you need to write a double backslash \\ !
        // - Our Emscripten build process allows embedding fonts to be
        // accessible at runtime from the "fonts/" folder. See
        // Makefile.emscripten for details. io.Fonts->AddFontDefault();
        // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
        // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        // ImFont* font =
        // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
        // nullptr, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font !=
        // nullptr);

        // Our state
        m_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    }
    ~MainApp() {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        ImPlot::DestroyContext();

        glfwDestroyWindow(m_main_window);
        glfwTerminate();
    }

    void RenderLoop();
};

}  // end namespace nfdrs
#endif  // !NFDRSGUI
