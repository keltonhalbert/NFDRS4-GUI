#ifndef MODEL_RUNNER_H
#define MODEL_RUNNER_H

#include <deadfuelmoisture.h>
#include <nfdrs4.h>

#include <atomic>
#include <memory>
#include <thread>

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
    std::unique_ptr<DeadFuelMoisture> model;
    DeadFuelSettings settings;

    DeadFuelModelRunner(double radius, const char* name) {
        model = std::make_unique<DeadFuelMoisture>(radius, name);
    }

    DeadFuelModelRunner(std::unique_ptr<DeadFuelMoisture>& in_model) {
        model = std::move(in_model);
    }
};

#endif
