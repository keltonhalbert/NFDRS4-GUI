#include <NFDRSGUI/FW21Decoder.h>
/*#include <NFDRSGUI/ModelRunners.h>*/
#include <NFDRSGUI/NFDRSGUI.h>

#include <cmath>
#include <cstddef>
#include <ctime>
#include <memory>

#include "imgui.h"
#include "implot.h"

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
        ImPlot::SetupAxes("Local Time", "deg F");
        /*// This first subplot should not have labels*/
        ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_NoLabel);
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
        ImPlot::SetupAxesLimits(stime[0], stime[N - 1], 32, 100);

        // X-axis constraints
        ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, stime[0], stime[N - 1]);
        ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 60 * 60 * 48,
                                         stime[N - 1] - stime[0]);

        // Y-axis constraints
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, -10, 150);
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y1, 10, 80);

        // Set up a shared Y axis for relative humidity
        ImPlot::SetupAxis(ImAxis_Y2, "Relative Humidity (%)",
                          ImPlotAxisFlags_AuxDefault);
        ImPlot::SetupAxisLimits(ImAxis_Y2, 0, 100);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y2, 0, 105);
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y2, 10, 100);

        // Plot the fire-wx categories
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
        ImVec4 elev_col = {1, 0.616, 0, 0.25};
        ImVec4 crit_col = {0.831, 0, 0, 0.25};
        ImVec4 extr_col = {0.765, 0.086, 0.8, 0.25};
        PlotFireWxCat("FireWx Cat", stime, firewx_cat, N, {0, 100}, elev_col,
                      crit_col, extr_col);

        // Plot the Relative Humidity
        ImVec4 color = ImPlot::GetColormapColor(8, ImPlotColormap_BrBG);
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 1);
        ImPlot::PushStyleColor(ImPlotCol_Line, color);
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
        ImPlot::PlotLine("RELH", stime, relh, N);
        ImPlot::PopStyleColor();
        ImPlot::PopStyleVar();

        // Plot the Air Temperature
        color = ImPlot::GetColormapColor(1, ImPlotColormap_RdBu);
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2);
        ImPlot::PushStyleColor(ImPlotCol_Line, color);
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
        ImPlot::SetupAxes("Local Time", "WSPD (mph)");
        ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_NoLabel);
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);

        // X-axis constraints
        ImPlot::SetupAxesLimits(stime[0], stime[N - 1], 0, 100);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, stime[0], stime[N - 1]);
        ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 60 * 60 * 48,
                                         stime[N - 1] - stime[0]);

        // Y-axis constraints
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0, 100);
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y1, 10, 100);

        // Set up a shared Y axis for wind direction
        ImPlot::SetupAxis(ImAxis_Y2, "Wind Direction",
                          ImPlotAxisFlags_AuxDefault);
        ImPlot::SetupAxisLimits(ImAxis_Y2, 0, 360);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y2, 0, 360);
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y2, 360, 360);

        // Plot the fire-wx categories
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
        ImVec4 elev_col = {1, 0.616, 0, 0.25};
        ImVec4 crit_col = {0.831, 0, 0, 0.25};
        ImVec4 extr_col = {0.765, 0.086, 0.8, 0.25};
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
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 2,
                                   ImPlot::GetColormapColor(1), IMPLOT_AUTO,
                                   ImPlot::GetColormapColor(1));
        ImPlot::PlotScatter("WDIR", stime, wdir, N);

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
        ImPlot::SetupAxes("Local Time", "Solar Radiation (W m^-2)");
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
        ImPlot::SetupAxis(ImAxis_Y2, "Hourly Rainfall (in)",
                          ImPlotAxisFlags_AuxDefault);
        ImPlot::SetupAxisLimits(ImAxis_Y2, 0, 1);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_Y2, 0, 12);
        ImPlot::SetupAxisZoomConstraints(ImAxis_Y2, 0.5, 12);

        // Plot the fire-wx categories
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
        ImVec4 elev_col = {1, 0.616, 0, 0.25};
        ImVec4 crit_col = {0.831, 0, 0, 0.25};
        ImVec4 extr_col = {0.765, 0.086, 0.8, 0.25};
        PlotFireWxCat("FireWx Cat", stime, firewx_cat, N, {0, 1200}, elev_col,
                      crit_col, extr_col);

        // Plot the solar radiation
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.5f);
        ImPlot::PushStyleColor(ImPlotCol_Fill, ImVec4(1, 0.867, 0.325, 1.0));
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
        // plot the line data
        ImPlot::PlotShaded("SRAD", stime, srad, N, -INFINITY);
        ImPlot::PopStyleColor();
        ImPlot::PopStyleVar();

        // Plot the rainfall sinze 00Z
        const ImVec4 rain_color = {0.043, 0.522, 0.49, 1.0};
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.5f);
        ImPlot::PushStyleColor(ImPlotCol_Fill, rain_color);
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
        ImPlot::PlotBars("RAIN", stime, precip, N, 60 * 60);
        ImPlot::PopStyleColor();
        ImPlot::PopStyleVar();

        ImPlot::EndPlot();
    }
}

static void dead_fuel(const double stime[], const DeadFuelModelRunner& dfm_1h,
                      const DeadFuelModelRunner& dfm_10h,
                      const DeadFuelModelRunner& dfm_100h,
                      const DeadFuelModelRunner& dfm_1000h, std::ptrdiff_t N) {
    if (ImPlot::BeginPlot("Dead Fuels")) {
        // We want a 24 hour clock
        ImPlot::GetStyle().Use24HourClock = true;
        // Set up our plot axes and constraints
        ImPlot::SetupAxes("Local Time", "deg C");
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

        ImPlotColormap cmap = ImPlotColormap_BrBG;
        ImPlot::PushColormap(cmap);
        // Plot the Relative Humidity
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 1);
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);
        if (dfm_1h.finished) {
            ImPlot::PushStyleColor(ImPlotCol_Line,
                                   ImPlot::SampleColormap(0.95));
            ImPlot::PlotLine("1h fm", stime, dfm_1h.radial_moisture.get(), N);
            ImPlot::PopStyleColor();
        }
        if (dfm_10h.finished) {
            ImPlot::PushStyleColor(ImPlotCol_Line,
                                   ImPlot::SampleColormap(0.85));
            ImPlot::PlotLine("10h fm", stime, dfm_10h.radial_moisture.get(), N);
            ImPlot::PopStyleColor();
        }
        if (dfm_100h.finished) {
            ImPlot::PushStyleColor(ImPlotCol_Line, ImPlot::SampleColormap(0.8));
            ImPlot::PlotLine("100h fm", stime, dfm_100h.radial_moisture.get(),
                             N);
            ImPlot::PopStyleColor();
        }
        if (dfm_1000h.finished) {
            ImPlot::PushStyleColor(ImPlotCol_Line,
                                   ImPlot::SampleColormap(0.75));
            ImPlot::PlotLine("1000h fm", stime, dfm_1000h.radial_moisture.get(),
                             N);
            ImPlot::PopStyleColor();
        }
        ImPlot::PopStyleVar();
        ImPlot::PopColormap();

        // Plot the fuel temperature
        cmap = ImPlotColormap_RdBu;
        ImPlot::PushColormap(cmap);
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 1);
        ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);
        if (dfm_1h.finished) {
            ImPlot::PushStyleColor(ImPlotCol_Line, ImPlot::SampleColormap(.2));
            ImPlot::PlotLine("1h ft", stime, dfm_1h.fuel_temperature.get(), N);
            ImPlot::PopStyleColor();
        }
        if (dfm_10h.finished) {
            ImPlot::PushStyleColor(ImPlotCol_Line, ImPlot::SampleColormap(.15));
            ImPlot::PlotLine("10h ft", stime, dfm_10h.fuel_temperature.get(),
                             N);
            ImPlot::PopStyleColor();
        }
        if (dfm_100h.finished) {
            ImPlot::PushStyleColor(ImPlotCol_Line, ImPlot::SampleColormap(.1));
            ImPlot::PlotLine("100h ft", stime, dfm_100h.fuel_temperature.get(),
                             N);
            ImPlot::PopStyleColor();
        }
        if (dfm_1000h.finished) {
            ImPlot::PushStyleColor(ImPlotCol_Line,
                                   ImPlot::SampleColormap(0.05));
            ImPlot::PlotLine("1000h ft", stime,
                             dfm_1000h.fuel_temperature.get(), N);
            ImPlot::PopStyleColor();
        }
        ImPlot::PopStyleVar();
        ImPlot::PopColormap();

        ImPlot::EndPlot();
    }
}

void meteogram(const std::unique_ptr<fw21::FW21Timeseries>& ts_data,
               const DeadFuelModelRunner& dfm_1h,
               const DeadFuelModelRunner& dfm_10h,
               const DeadFuelModelRunner& dfm_100h,
               const DeadFuelModelRunner& dfm_1000h,
               const ImVec2 resize_thresh) {
    const ImVec2 window_size = ImGui::GetWindowSize();
    ImVec2 plot_size = {-1, -1};
    int rows = 3;
    int cols = 2;
    /*if ((window_size.x < resize_thresh.x) ||*/
    /*    (window_size.y < resize_thresh.y)) {*/
    /*    rows = 6;*/
    /*    cols = 1;*/
    /*    plot_size.y = 2048;*/
    /*}*/
    if (ImPlot::BeginSubplots(
            "Station Meteogram", rows, cols, plot_size,
            ImPlotSubplotFlags_LinkAllX | ImPlotSubplotFlags_ColMajor)) {
        if (ts_data) {
            temperature_and_humidity(ts_data->date_time.data(),
                                     ts_data->air_temperature.data(),
                                     ts_data->relative_humidity.data(),
                                     ts_data->spc_cat.data(), ts_data->NT);
            surface_winds(ts_data->date_time.data(), ts_data->wind_speed.data(),
                          ts_data->wind_direction.data(),
                          ts_data->gust_speed.data(), ts_data->spc_cat.data(),
                          ts_data->NT);
            solar_radiation_and_precip(ts_data->date_time.data(),
                                       ts_data->solar_radiation.data(),
                                       ts_data->precipitation.data(),
                                       ts_data->spc_cat.data(), ts_data->NT);

            dead_fuel(ts_data->date_time.data(), dfm_1h, dfm_10h, dfm_100h,
                      dfm_1000h, ts_data->NT);
            dead_fuel(ts_data->date_time.data(), dfm_1h, dfm_10h, dfm_100h,
                      dfm_1000h, ts_data->NT);
            dead_fuel(ts_data->date_time.data(), dfm_1h, dfm_10h, dfm_100h,
                      dfm_1000h, ts_data->NT);
        }

        ImPlot::EndSubplots();
    }
}

}  // namespace nfdrs
