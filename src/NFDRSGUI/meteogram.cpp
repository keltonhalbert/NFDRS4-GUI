#include <imgui.h>
#include <implot.h>

#include <cstddef>

namespace nfdrs {

void meteogram(const double stime[], const double tmpc[], const double relh[],
               std::ptrdiff_t N) {
    const auto window_size = ImGui::GetWindowSize();
    if (ImPlot::BeginPlot("Station Meteogram", window_size)) {
        // We want a 24 hour clock
        ImPlot::GetStyle().Use24HourClock = true;
        // Set up our plot axes and constraints
        ImPlot::SetupAxes("UTC Time", "deg C");
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
        ImPlot::SetupAxesLimits(stime[0], stime[N - 1], 0, 35);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, stime[0], stime[N - 1]);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, -10, 50);

        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2);
        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0, 0.0, 0.0, 1.0));
        // plot the line data
        ImPlot::PlotLine("TAIR", stime, tmpc, N);
        ImPlot::PopStyleColor();
        ImPlot::PopStyleVar();
        ImPlot::EndPlot();
    }
}

}  // namespace nfdrs
