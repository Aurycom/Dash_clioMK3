// MultiGpioMonitorQt.hpp
#pragma once

#include "GpioMonitorQt.hpp"
#include <QObject>
#include <QVector>
#include <functional>

class MultiGpioMonitorQt : public QObject {
    Q_OBJECT

public:
    explicit MultiGpioMonitorQt(QObject* parent = nullptr);
    ~MultiGpioMonitorQt();

    bool add(const QString& chipname, unsigned int line_offset,
             std::function<void(unsigned int, bool)> callback);

    void start_all();
    void stop_all();

private:
    struct MonitorEntry {
        GpioMonitorQt* monitor;
        unsigned int line_offset;
        std::function<void(unsigned int, bool)> callback;
    };

    QVector<MonitorEntry> monitors_;
};

