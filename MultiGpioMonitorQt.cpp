// MultiGpioMonitorQt.cpp
#include "MultiGpioMonitorQt.hpp"

MultiGpioMonitorQt::MultiGpioMonitorQt(QObject* parent)
    : QObject(parent)
{}

MultiGpioMonitorQt::~MultiGpioMonitorQt() {
    std::cout << "MultiGpioMonitorQt Destructeur "<<std::endl<<std::flush;
    stop_all(); // nettoyage
}

bool MultiGpioMonitorQt::add(const QString& chipname,
                             unsigned int line_offset,
                             std::function<void(unsigned int, bool)> callback)
{
    auto monitor = new GpioMonitorQt(chipname, line_offset, this);

    // Capture le line_offset dans le callback
    connect(monitor, &GpioMonitorQt::gpioChanged, this, [callback, line_offset](bool val) {
        callback(line_offset, val);
    });

    monitors_.append({ monitor, line_offset, callback });
    return true;
}

void MultiGpioMonitorQt::start_all() {
    for (auto& entry : monitors_)
        entry.monitor->start();
}

void MultiGpioMonitorQt::stop_all() {
    std::cout << "MultiGpioMonitorQt stop_all "<<std::endl<<std::flush;
    for (auto& entry : monitors_)
        entry.monitor->stop();
}

