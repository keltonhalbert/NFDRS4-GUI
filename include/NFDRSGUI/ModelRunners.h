#ifndef MODEL_RUNNER_H
#define MODEL_RUNNER_H

#include <deadfuelmoisture.h>
#include <nfdrs4.h>

#include <atomic>
#include <memory>
#include <thread>

template <typename ModelType>
struct AsyncModelRunner {
    std::unique_ptr<ModelType> model;
};

#endif
