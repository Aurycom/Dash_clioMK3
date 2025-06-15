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
#include "AFFA2EmulatorDisplay.hpp"
#include <QTimer>

#define ClioMK3_LOG(severity) BOOST_LOG_TRIVIAL(severity) << "[ClioMK3Plugin] "

enum AFFA2Source {
    sourceFM = 0,
    sourceCD,
    sourceCDChanger,
    sourceAUX,
    sourceUnknown
};

class InfoWindow : public QWidget {
    Q_OBJECT
    public:
        InfoWindow(Arbiter &arbiter, QWidget *parent = nullptr);
        QLabel* text;
        QLabel* menu;
        QLabel* blink;
        QLabel* source;
        QLabel* news;
        QLabel* rds;
        QLabel* traffic;
        QLabel* updateListMode;
        QLabel* presetMode;
        QLabel* manualMode;
        QList<QString> listText;
};

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
		AFFA2EmulatorDisplay *display;
        AAHandler *aa_handler;
        Vehicle *vehicle;
        InfoWindow *info;
        enum AFFA2Source currentSource;
        int currentBlinkPosition;
        //void switchToSource(enum AFFA2Source source);
        void updateSource(QString text);
        void updateSource(enum AFFA2Source source);

    public slots:
        void updateText(QString text);
        void updateMenu(QString text);
        void showMenu();
        void hideMenu();
        void startBlinkText(QString text);
        void stopBlinkText();
        void updateBlinkText(QString text);
        void blink();
        void radioIconsChanged(bool news, bool traffic, bool afrds);
        void modeChanged(enum AFFA2Mode mode);

};

