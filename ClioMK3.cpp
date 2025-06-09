#include "ClioMK3.hpp"

#define DEBUG false


ClioMK3::~ClioMK3()
{
    if (this->climate)
        delete this->climate;
    if (this->vehicle)
        delete this->vehicle;
}

bool ClioMK3::init(ICANBus* canbus){
	this->duelClimate=false;
	 if (this->arbiter) {
        this->aa_handler = this->arbiter->android_auto().handler;
		this->vehicle = new Vehicle(*this->arbiter);
        this->vehicle->setObjectName("Clio 3 Phase 1");
        this->display = new AFFA2Emulator();
		canbus->registerFrameHandler(0x121, [this](QByteArray payload){this->display->getText(payload);});
        connect(this->display, SIGNAL(displayTextChanged(QString)), this, SLOT(updateText(QString)));
	 }
		
}

QList<QWidget *> ClioMK3::widgets()
{
    QList<QWidget *> tabs;
    tabs.append(this->vehicle);
    if(DEBUG)
        tabs.append(this->debug);
    return tabs;
}

void ClioMK3::updateText(QString text) {
    ClioMK3_LOG(info)<<text;
}