#ifndef MODEL_RUNNER_H
#define MODEL_RUNNER_H

#include <deadfuelmoisture.h>
#include <nfdrs4.h>

#include <atomic>
#include <memory>
#include <thread>

struct DeadFuelModelRunner {
    std::unique_ptr<DeadFuelMoisture> model;

    DeadFuelModelRunner(std::unique_ptr<DeadFuelMoisture>& in_model) {
        model = std::move(in_model);
    }
};

#endif
