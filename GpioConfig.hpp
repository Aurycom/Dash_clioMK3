// GpioConfig.hpp
#pragma once
#include <string>
#include <functional>

struct GpioConfig {
    std::string chipname;
    unsigned int line_offset;
    std::function<void(bool)> callback;
};
