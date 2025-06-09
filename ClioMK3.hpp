#include <QString>
#include <iostream>
#include <QWidget>
#include <stdlib.h>
#include <QByteArray>
#include <boost/log/trivial.hpp>
#include "app/arbiter.hpp"
#include "app/widgets/vehicle.hpp"
#include "openauto/Service/InputService.hpp"
#include "AAHandler.hpp"
#include "plugins/vehicle_plugin.hpp"
#include "AFFA2emulator.hpp"

#define ClioMK3_LOG(severity) BOOST_LOG_TRIVIAL(severity) << "[ClioMK3Plugin] "

class ClioMK3 : public QObject, VehiclePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID VehiclePlugin_iid)
    Q_INTERFACES(VehiclePlugin)
    public:
        ClioMK3() {};
        ~ClioMK3();
        bool init(ICANBus* canbus) override;
		QList<QWidget *> widgets() override;
    private:
		  AFFA2Emulator display;

    private slots:
      void updateText(QString text);

};