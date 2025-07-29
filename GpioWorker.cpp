// GpioWorker.cpp
#include "GpioWorker.hpp"
#include <iostream>
#include <thread>

GpioWorker::GpioWorker(const std::string& chipname, unsigned int line_offset)
    : chipname_(chipname),
      line_offset_(line_offset),
      chip_(chipname),
      line_(chip_.get_line(line_offset)),
      running_(false)
{
}

GpioWorker::~GpioWorker() {
    std::cout << "GpioWorker stop"<<std::endl<<std::flush;
    stop();
}

void GpioWorker::start() {
    if (running_) return;

    try {
        line_.request({ "gpio-monitor", gpiod::line_request::EVENT_BOTH_EDGES });
    } catch (const std::exception& e) {
        qWarning("Erreur de requête GPIO: %s", e.what());
        return;
    }

    running_ = true;
    QtConcurrent::run(this, &GpioWorker::monitor_loop);  // Qt thread-safe
}

void GpioWorker::stop() {
    running_ = false;
    if (line_.is_requested()) {
        line_.release();
    }
    std::cout << "end stop";
}

void GpioWorker::monitor_loop() {
    while (running_) {
        try {
            if (!line_.event_wait(std::chrono::milliseconds(100)))
                continue;

            auto evt = line_.event_read();
            bool value = line_.get_value();

            emit gpioChanged(value);

        } catch (const std::exception& e) {
            qWarning("Erreur boucle GPIO: %s", e.what());
        }
    }
    std::cout << "end while";
}

