// GpioMonitorQt.hpp
#pragma once

#include "GpioWorker.hpp"
#include <QObject>
#include <QThread>
#include <iostream>

class GpioMonitorQt : public QObject {
    Q_OBJECT

public:
    GpioMonitorQt(const QString& chipname, unsigned int line_offset, QObject* parent = nullptr);
    ~GpioMonitorQt();

    void start();
    void stop();

signals:
    void gpioChanged(bool value);  // signal émis par le worker

private:
    QThread thread_;
    GpioWorker* worker_;
};

