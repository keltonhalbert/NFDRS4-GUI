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
    float max_local_moisture = 0.6;
    float stick_density = 0.4;
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
    double radius;
    std::string name;
    std::thread process_thread;
    DeadFuelSettings settings;
    std::unique_ptr<DeadFuelMoisture> model;
    std::unique_ptr<double[]> radial_moisture;
    std::unique_ptr<double[]> fuel_temperature;
    std::atomic<int> progress = 0;
    const std::ptrdiff_t size;

    DeadFuelModelRunner(double in_radius, const char* in_name,
                        const Meteogram& data)
        : size(data.N), process_thread() {
        radius = in_radius;
        name = in_name;
        model = std::make_unique<DeadFuelMoisture>(radius, name);
        radial_moisture = std::make_unique<double[]>(size);
        fuel_temperature = std::make_unique<double[]>(size);

        // get derived settings
        settings.adsorption_rate = model->adsorptionRate();
        settings.desorption_rate = model->desorptionRate();
        settings.planar_heat_transfer_rate = model->planarHeatTransferRate();
        settings.diffusivity_steps = model->diffusivitySteps();
        settings.moisture_steps = model->moistureSteps();
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
            fuel_temperature[i] = model->meanWtdTemperature();
            progress.store(100 * i / data.N);
        }
    }

    void run(const Meteogram& data) {
        model->setRandomSeed(settings.random_seed);
        model->setDiffusivitySteps(settings.diffusivity_steps);
        model->setMoistureSteps(settings.moisture_steps);
        model->setStickNodes(settings.stick_nodes);
        model->setAdsorptionRate(settings.adsorption_rate);
        model->setDesorptionRate(settings.desorption_rate);
        model->setPlanarHeatTransferRate(settings.planar_heat_transfer_rate);
        model->setStickLength(settings.stick_length);
        model->setStickDensity(settings.stick_density);
        model->setMaximumLocalMoisture(settings.max_local_moisture);
        model->initializeStick();
        process_thread =
            std::thread(&DeadFuelModelRunner::calc_dfm, this, std::ref(data));
    }

    void default_settings() {
        settings = DeadFuelSettings();
        settings.adsorption_rate = model->deriveAdsorptionRate(radius);
        settings.diffusivity_steps = model->deriveDiffusivitySteps(radius);
        settings.moisture_steps = model->deriveMoistureSteps(radius);
        settings.stick_nodes = model->deriveStickNodes(radius);
        settings.planar_heat_transfer_rate =
            model->derivePlanarHeatTransferRate(radius);
    }

    void reset() {
        if (process_thread.joinable()) process_thread.join();
        process_thread = std::thread();
        progress = 0;
        model->initializeParameters(radius, name);
    }
};

}  // namespace nfdrs

#endif
