#include <imgui.h>
#include <implot.h>
#include <implot_internal.h>

#include <cmath>
#include <cstddef>

namespace nfdrs {

static void humidity(const double stime[], const double dfm_1h[],
                     const double dfm_10h[], const double dfm_100h[],
                     const double dfm_1000h[], std::ptrdiff_t N) {
    const auto window_size = ImGui::GetWindowSize();
    if (ImPlot::BeginPlot("Fuel Moisture", window_size)) {
        // We want a 24 hour clock
        ImPlot::GetStyle().Use24HourClock = true;
        // Set up our plot axes and constraints
        ImPlot::SetupAxes("UTC Time", "Humidity (%)");
        // This first subplot should not have labels
        ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_NoLabel);
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
        ImPlot::SetupAxesLimits(stime[0], stime[N - 1], 0, 100);

        // X-axis constraints
        ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, stime[0], stime[N - 1]);
        ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 60 * 60 * 48,
                                         stime[N - 1] - stime[0]);

        // Y-axis constraints
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, 100);
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y1, 5, 100);

        // Plot the Relative Humidity
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 1);
        /*ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.0, 0.70, 0.0, 1.0));*/
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
        ImPlot::PlotLine("1h", stime, dfm_1h, N);
        ImPlot::PlotLine("10h", stime, dfm_10h, N);
        ImPlot::PlotLine("100h", stime, dfm_100h, N);
        ImPlot::PlotLine("1000h", stime, dfm_1000h, N);
        /*ImPlot::PopStyleColor();*/
        ImPlot::PopStyleVar();

        ImPlot::EndPlot();
    }
}

void fuel_moisture_timeseries(const double stime[], const double dfm_1h[],
                              const double dfm_10h[], const double dfm_100h[],
                              const double dfm_1000h[], std::ptrdiff_t N) {
    /*const auto window_size = ImGui::GetWindowSize();*/
    /*const int rows = 1;*/
    /*const int cols = 1;*/
    /*if (ImPlot::BeginSubplots("Dead Fuel Meteogram", rows, cols,
     * window_size,*/
    /*                          ImPlotSubplotFlags_LinkAllX)) {*/
    humidity(stime, dfm_1h, dfm_10h, dfm_100h, dfm_1000h, N);

    /*    ImPlot::EndSubplots();*/
    /*}*/
}

}  // namespace nfdrs
