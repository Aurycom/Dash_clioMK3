// GpioWorker.hpp
#pragma once

#include <QObject>
#include <gpiod.hpp>
#include <atomic>
#include <chrono>
#include <QtConcurrent>

class GpioWorker : public QObject {
    Q_OBJECT

public:
    GpioWorker(const std::string& chipname, unsigned int line_offset);
    ~GpioWorker();

public slots:
    void start();
    void stop();

signals:
    void gpioChanged(bool value);

private:
    void monitor_loop();

    std::string chipname_;
    unsigned int line_offset_;
    gpiod::chip chip_;
    gpiod::line line_;
    std::atomic<bool> running_;
};

