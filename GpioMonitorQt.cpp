// GpioMonitorQt.cpp
#include "GpioMonitorQt.hpp"

GpioMonitorQt::GpioMonitorQt(const QString& chipname, unsigned int line_offset, QObject* parent)
    : QObject(parent)
{
    worker_ = new GpioWorker(chipname.toStdString(), line_offset);
    worker_->moveToThread(&thread_);

    connect(&thread_, &QThread::finished, worker_, &QObject::deleteLater);
    connect(worker_, &GpioWorker::gpioChanged, this, &GpioMonitorQt::gpioChanged);

    thread_.start();
}

GpioMonitorQt::~GpioMonitorQt() {
    std::cout << "GpioMonitorQt stop"<<std::endl<<std::flush;
    stop();
    thread_.quit();
    thread_.wait();
}

void GpioMonitorQt::start() {
    QMetaObject::invokeMethod(worker_, "start", Qt::QueuedConnection);
}

void GpioMonitorQt::stop() {
    QMetaObject::invokeMethod(worker_, "stop", Qt::QueuedConnection);
}

