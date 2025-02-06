#ifndef MODEL_RUNNER_H
#define MODEL_RUNNER_H

#include <NFDRSGUI/Meteogram.h>
#include <deadfuelmoisture.h>
#include <nfdrs4.h>

#include <atomic>
#include <cstddef>
#include <memory>
#include <thread>

#include "implot.h"
#include "implot_internal.h"

namespace nfdrs {
struct DeadFuelSettings {
    double adsorption_rate;
    double desorption_rate = 0.06;
    double planar_heat_transfer_rate;
    double max_local_moisture = 0.6;
    double stick_density = 0.4;
    double stick_length = 41.0;
    int diffusivity_steps;
    int moisture_steps;
    int stick_nodes = 11;
    int random_seed = 0;
    bool use_derived_adsorption_rate = true;
    bool use_derived_diffusivity_steps = true;
    bool use_derived_moisture_steps = true;
    bool use_derived_planar_hear_transfer_rate = true;
    bool use_derived_stick_nodes = true;
};

struct DeadFuelModelRunner {
    std::thread process_thread;
    DeadFuelSettings settings;
    std::unique_ptr<DeadFuelMoisture> model;
    std::unique_ptr<double[]> radial_moisture;
    std::unique_ptr<double[]> fuel_temperature;
    std::atomic<int> progress = 0;
    const std::ptrdiff_t size;

    DeadFuelModelRunner(double radius, const char* name, const Meteogram& data)
        : size(data.N), process_thread() {
        model = std::make_unique<DeadFuelMoisture>(radius, name);
        radial_moisture = std::make_unique<double[]>(size);
        fuel_temperature = std::make_unique<double[]>(size);
    }

    DeadFuelModelRunner(std::unique_ptr<DeadFuelMoisture>& in_model,
                        const Meteogram& data)
        : size(data.N) {
        model = std::move(in_model);
        radial_moisture = std::make_unique<double[]>(size);
        fuel_temperature = std::make_unique<double[]>(size);
    }

    ~DeadFuelModelRunner() {
        if (process_thread.joinable()) process_thread.join();
    }

    void calc_dfm(const Meteogram& data) {
        for (int i = 0; i < data.N; ++i) {
            double at = data.m_tair[i];
            double rh = data.m_relh[i] / 100.0;
            double sW = data.m_srad[i];
            double rain = data.m_rain[i] * 10.0;
            tm time_data;
            ImPlotTime curtime = ImPlotTime::FromDouble(data.m_timestamp[i]);
            ImPlot::GetGmtTime(curtime, &time_data);

            bool ret = model->update(
                time_data.tm_year + 1900, time_data.tm_mon + 1,
                time_data.tm_mday, time_data.tm_hour, time_data.tm_min,
                time_data.tm_sec, at, rh, sW, rain, 0.0218, true);
            radial_moisture[i] = model->medianRadialMoisture() * 100.0;
            progress.store(100 * i / data.N);
        }
    }

    void run(const Meteogram& data) {
        process_thread =
            std::thread(&DeadFuelModelRunner::calc_dfm, this, std::ref(data));
    }
};

}  // namespace nfdrs

#endif
