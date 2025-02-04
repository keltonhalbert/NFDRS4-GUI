#include <imgui.h>
#include <implot.h>
#include <implot_internal.h>

#include <cmath>
#include <cstddef>

namespace nfdrs {

static void PlotFireWxCat(const char* label_id, const double* xs,
                          const int* categories, int count, const ImVec2 bounds,
                          const ImVec4 elev_col, const ImVec4 crit_col,
                          const ImVec4 extr_col) {
    ImDrawList* draw_list = ImPlot::GetPlotDrawList();

    if (ImPlot::BeginItem(label_id)) {
        int prev_fire_cat = 0;
        ImVec2 start_pos;
        ImVec2 end_pos;
        int start_idx, end_idx;
        ImPlot::GetCurrentItem()->Color = IM_COL32(64, 64, 64, 255);
        for (int idx = 0; idx < count; ++idx) {
            const int fire_cat = categories[idx];

            if ((fire_cat > 0) && (prev_fire_cat == 0)) {
                start_idx = idx;
                start_pos = ImPlot::PlotToPixels(xs[idx], bounds.x);
            } else if ((fire_cat > 0) && (prev_fire_cat != 0) &&
                       (prev_fire_cat != fire_cat)) {
                end_idx = idx;
                end_pos = ImPlot::PlotToPixels(xs[idx], bounds.y);

                // What was the max category?
                int max_cat = 0;
                for (int i = start_idx; i < end_idx; ++i) {
                    if (categories[i] > max_cat) max_cat = prev_fire_cat;
                }

                ImU32 col;
                if (max_cat == 1) {
                    col = ImGui::GetColorU32(elev_col);
                } else if (max_cat == 2) {
                    col = ImGui::GetColorU32(crit_col);
                } else if (max_cat == 3) {
                    col = ImGui::GetColorU32(extr_col);
                }

                draw_list->AddRectFilled(start_pos, end_pos, col);
                start_pos = ImPlot::PlotToPixels(xs[idx], bounds.x);
                start_idx = idx;

            } else if ((fire_cat == 0) && (prev_fire_cat != 0)) {
                end_pos = ImPlot::PlotToPixels(xs[idx], bounds.y);
                end_idx = idx;

                // What was the max category?
                int max_cat = 0;
                for (int i = start_idx; i < end_idx; ++i) {
                    if (categories[i] > max_cat) max_cat = prev_fire_cat;
                }

                ImU32 col;
                if (max_cat == 1) {
                    col = ImGui::GetColorU32(elev_col);
                } else if (max_cat == 2) {
                    col = ImGui::GetColorU32(crit_col);
                } else if (max_cat == 3) {
                    col = ImGui::GetColorU32(extr_col);
                }

                draw_list->AddRectFilled(start_pos, end_pos, col);
            }
            prev_fire_cat = fire_cat;
        }
        ImPlot::EndItem();
    }
}

static void temperature_and_humidity(const double stime[], const double tmpc[],
                                     const double relh[],
                                     const int firewx_cat[], std::ptrdiff_t N) {
    if (ImPlot::BeginPlot("Air Temperature and Humidity")) {
        // We want a 24 hour clock
        ImPlot::GetStyle().Use24HourClock = true;
        // Set up our plot axes and constraints
        ImPlot::SetupAxes("UTC Time", "deg C");
        // This first subplot should not have labels
        ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_NoLabel);
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
        ImPlot::SetupAxesLimits(stime[0], stime[N - 1], 0, 35);

        // X-axis constraints
        ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, stime[0], stime[N - 1]);
        ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 60 * 60 * 48,
                                         stime[N - 1] - stime[0]);

        // Y-axis constraints
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, -10, 50);
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y1, 5, 50);

        // Set up a shared Y axis for relative humidity
        ImPlot::SetupAxis(ImAxis_Y2, "Relative Humidity (%)",
                          ImPlotAxisFlags_AuxDefault);
        ImPlot::SetupAxisLimits(ImAxis_Y2, 0, 100);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y2, 0, 100);
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y2, 10, 100);

        // Plot the fire-wx categories
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
        ImVec4 elev_col = {1, 0.616, 0, 0.5};
        ImVec4 crit_col = {0.831, 0, 0, 0.5};
        ImVec4 extr_col = {0.765, 0.086, 0.8, 0.5};
        PlotFireWxCat("FireWx Cat", stime, firewx_cat, N, {0, 100}, elev_col,
                      crit_col, extr_col);

        // Plot the Relative Humidity
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 1);
        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.0, 0.70, 0.0, 1.0));
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
        ImPlot::PlotLine("RELH", stime, relh, N);
        ImPlot::PopStyleColor();
        ImPlot::PopStyleVar();

        // Plot the Air Temperature
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2);
        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.0, 0.0, 0.0, 1.0));
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
        // plot the line data
        ImPlot::PlotLine("TAIR", stime, tmpc, N);
        ImPlot::PopStyleColor();
        ImPlot::PopStyleVar();

        ImPlot::EndPlot();
    }
}

static void surface_winds(const double stime[], const double wspd[],
                          const double wdir[], const double gust[],
                          const int firewx_cat[], std::ptrdiff_t N) {
    if (ImPlot::BeginPlot("10m Winds")) {
        // We want a 24 hour clock
        ImPlot::GetStyle().Use24HourClock = true;
        // Set up our plot axes and constraints
        ImPlot::SetupAxes("UTC Time", "WSPD (m/s)");
        ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_NoLabel);
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);

        // X-axis constraints
        ImPlot::SetupAxesLimits(stime[0], stime[N - 1], 0, 20);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, stime[0], stime[N - 1]);
        ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 60 * 60 * 48,
                                         stime[N - 1] - stime[0]);

        // Y-axis constraints
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, 50);
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y1, 10, 50);

        // Set up a shared Y axis for wind direction
        ImPlot::SetupAxis(ImAxis_Y2, "Wind Direction",
                          ImPlotAxisFlags_AuxDefault);
        ImPlot::SetupAxisLimits(ImAxis_Y2, 0, 360);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y2, 0, 360);
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y2, 360, 360);

        // Plot the fire-wx categories
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
        ImVec4 elev_col = {1, 0.616, 0, 0.5};
        ImVec4 crit_col = {0.831, 0, 0, 0.5};
        ImVec4 extr_col = {0.765, 0.086, 0.8, 0.5};
        PlotFireWxCat("FireWx Cat", stime, firewx_cat, N, {0, 100}, elev_col,
                      crit_col, extr_col);

        // Plot the wind gust
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.5f);
        ImPlot::PushStyleColor(ImPlotCol_Fill,
                               ImVec4(0.102, 0.537, 0.769, 1.0));
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
        // plot the line data
        ImPlot::PlotShaded("WMAX", stime, gust, wspd, N);
        ImPlot::PopStyleColor();
        ImPlot::PopStyleVar();

        // Plot the wind speed
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.5f);
        ImPlot::PushStyleColor(ImPlotCol_Fill, ImVec4(0.04, 0.254, 0.368, 1.0));
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
        // plot the line data
        ImPlot::PlotShaded("WSPD", stime, wspd, N, -INFINITY);
        ImPlot::PopStyleColor();
        ImPlot::PopStyleVar();

        // Plot the wind direction
        /*ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 1);*/
        /*ImPlot::PushStyleColor(ImPlotCol_Line,
         * ImVec4(1.0, 1.0, 1.0, 1.0));*/
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 2,
                                   ImPlot::GetColormapColor(1), IMPLOT_AUTO,
                                   ImPlot::GetColormapColor(1));
        ImPlot::PlotScatter("WDIR", stime, wdir, N);
        /*ImPlot::PopStyleColor();*/
        /*ImPlot::PopStyleVar();*/

        ImPlot::EndPlot();
    }
}

static void solar_radiation_and_precip(const double stime[],
                                       const double srad[],
                                       const double precip[],
                                       const int firewx_cat[],
                                       std::ptrdiff_t N) {
    if (ImPlot::BeginPlot("Solar Radiation and Precipitation")) {
        // We want a 24 hour clock
        ImPlot::GetStyle().Use24HourClock = true;
        // Set up our plot axes and constraints
        ImPlot::SetupAxes("UTC Time", "Solar Radiation (W m^-2)");
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);

        // X-axis constraints
        ImPlot::SetupAxesLimits(stime[0], stime[N - 1], 0, 1200);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, stime[0], stime[N - 1]);
        ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 60 * 60 * 48,
                                         stime[N - 1] - stime[0]);

        // Y-axis constraints
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, 1200);
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y1, 100, 1200);

        // Set up a shared Y axis for rainfall
        ImPlot::SetupAxis(ImAxis_Y2, "Hourly Rainfall (mm)",
                          ImPlotAxisFlags_AuxDefault);
        ImPlot::SetupAxisLimits(ImAxis_Y2, 0, 100);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y2, 0, 600);
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y2, 10, 600);

        // Plot the fire-wx categories
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
        ImVec4 elev_col = {1, 0.616, 0, 0.5};
        ImVec4 crit_col = {0.831, 0, 0, 0.5};
        ImVec4 extr_col = {0.765, 0.086, 0.8, 0.5};
        PlotFireWxCat("FireWx Cat", stime, firewx_cat, N, {0, 1200}, elev_col,
                      crit_col, extr_col);

        // Plot the solar radiation
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.5f);
        ImPlot::PushStyleColor(ImPlotCol_Fill, ImVec4(0.941, 0.82, 0.282, 1.0));
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
        // plot the line data
        ImPlot::PlotShaded("SRAD", stime, srad, N, -INFINITY);
        ImPlot::PopStyleColor();
        ImPlot::PopStyleVar();

        // Plot the rainfall sinze 00Z
        const ImVec4 rain_color = {0.243, 0.722, 0.322, 1.0};
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.5f);
        ImPlot::PushStyleColor(ImPlotCol_Fill, rain_color);
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
        ImPlot::PlotBars("RAIN", stime, precip, N, 60 * 60);
        ImPlot::PopStyleColor();
        ImPlot::PopStyleVar();

        ImPlot::EndPlot();
    }
}

static void dead_fuel(const double stime[], const double dfm_1h[],
                      const double dfm_10h[], const double dfm_100h[],
                      const double dfm_1000h[], std::ptrdiff_t N) {
    if (ImPlot::BeginPlot("Fuel Moisture")) {
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

void meteogram(const double stime[], const double tmpc[], const double relh[],
               const double wspd[], const double wdir[], const double gust[],
               const double precip[], const double srad[],
               const int firewx_cat[], const double dfm_1h[],
               const double dfm_10h[], const double dfm_100h[],
               const double dfm_1000h[], std::ptrdiff_t N) {
    const auto window_size = ImGui::GetWindowSize();
    const int rows = 3;
    const int cols = 2;
    if (ImPlot::BeginSubplots(
            "Station Meteogram", rows, cols, {-1, -1},
            ImPlotSubplotFlags_LinkAllX | ImPlotSubplotFlags_ColMajor)) {
        temperature_and_humidity(stime, tmpc, relh, firewx_cat, N);
        surface_winds(stime, wspd, wdir, gust, firewx_cat, N);
        solar_radiation_and_precip(stime, srad, precip, firewx_cat, N);

        dead_fuel(stime, dfm_1h, dfm_10h, dfm_100h, dfm_1000h, N);
        dead_fuel(stime, dfm_1h, dfm_10h, dfm_100h, dfm_1000h, N);
        dead_fuel(stime, dfm_1h, dfm_10h, dfm_100h, dfm_1000h, N);

        ImPlot::EndSubplots();
    }
}

}  // namespace nfdrs
