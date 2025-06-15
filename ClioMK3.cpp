#include "ClioMK3.hpp"

#define DEBUG false

InfoWindow::InfoWindow(Arbiter &arbiter, QWidget *parent) : QWidget(parent)
{

    QVBoxLayout *globalLayout = new QVBoxLayout(this);

    QHBoxLayout *layoutSource = new QHBoxLayout(this);
    source = new QLabel("", this);
    layoutSource->addWidget(source);
    globalLayout->addLayout(layoutSource);

    QHBoxLayout *layout = new QHBoxLayout(this);
    text = new QLabel("--", this);
    menu = new QLabel("", this);
    blink = new QLabel("", this);
    menu->setVisible(false);
    blink->setVisible(false);
    layout->addWidget(text);
    layout->addWidget(menu);
    layout->addWidget(blink);
    globalLayout->addLayout(layout);

    QHBoxLayout *layoutRadioItems = new QHBoxLayout(this);
    rds = new QLabel("AF-RDS", this);
    rds->setVisible(false);
    news = new QLabel("iNews", this);
    news->setVisible(false);
    traffic = new QLabel("iTraffic", this);
    traffic->setVisible(false);
    layoutRadioItems->addWidget(rds);
    layoutRadioItems->addWidget(news);
    layoutRadioItems->addWidget(traffic);
    globalLayout->addLayout(layoutRadioItems);

    QHBoxLayout *layoutModeItems = new QHBoxLayout(this);
    updateListMode = new QLabel("Update List", this);
    updateListMode->setVisible(false);
    presetMode = new QLabel("Preset Mode", this);
    presetMode->setVisible(false);
    manualMode = new QLabel("Manual Mode", this);
    manualMode->setVisible(false);
    layoutModeItems->addWidget(updateListMode);
    layoutModeItems->addWidget(presetMode);
    layoutModeItems->addWidget(manualMode);
    globalLayout->addLayout(layoutModeItems);

}

ClioMK3::~ClioMK3()
{
    /*if (this->vehicle)
        delete this->vehicle;*/
    currentBlinkPosition=0;
}

bool ClioMK3::init(ICANBus* canbus){
	 if (this->arbiter) {
        this->aa_handler = this->arbiter->android_auto().handler;
		/*this->vehicle = new Vehicle(*this->arbiter);
        this->vehicle->setObjectName("Clio 3 Phase 1");*/

        this->info = new InfoWindow(*this->arbiter);
        this->info->setObjectName("Info");

        this->display = new AFFA2EmulatorDisplay();
		canbus->registerFrameHandler(0x121, [this](QByteArray payload){this->display->getText(payload);});

        connect(this->display, SIGNAL(displayTextChanged(QString)), this, SLOT(updateText(QString)));
        connect(this->display, SIGNAL(displayMenuItemUpdate(QString)), this, SLOT(updateMenu(QString)));
        connect(this->display, SIGNAL(displayMenuShow()), this, SLOT(showMenu()));
        connect(this->display, SIGNAL(displayMenuHide()), this, SLOT(hideMenu()));

        connect(this->display, SIGNAL(addBlinkText(QString)), this, SLOT(updateBlinkText(QString)));
        connect(this->display, SIGNAL(stopBlinkText()), this, SLOT(stopBlinkText()));
        connect(this->display, SIGNAL(radioIconsChanged(bool, bool, bool)), this, SLOT(radioIconsChanged(bool, bool, bool)));
        connect(this->display, SIGNAL(modeChanged(enum AFFA2Mode)), this, SLOT(modeChanged(enum AFFA2Mode)));

        ClioMK3_LOG(info)<<"init success";

        return true;
	 }else{
        ClioMK3_LOG(error)<<"Failed to get arbiter";
        return false;
     }

}

QList<QWidget *> ClioMK3::widgets()
{
    QList<QWidget *> tabs;
    //tabs.append(this->vehicle);
    /*if(DEBUG)
        tabs.append(this->debug);*/
    tabs.append(this->info);
    return tabs;
}

void ClioMK3::updateText(QString text) {
    updateSource(text);
    this->info->text->setText(text);
    ClioMK3_LOG(info) << "[updateText] " << text.toStdString();
}

void ClioMK3::updateSource(QString text){

    enum AFFA2Source source = sourceUnknown;

    if (text.isEmpty()) { /* Radio wyłączone */
        currentSource = sourceUnknown;
    }

    if (text == "AUX         ") {
        source = sourceAUX;
        text = "BLUETOOTH   "; /* Podmieniamy AUX na BLUETOOTH */
    }
    else if ((text == "CD          ") || (text == "  LOAD CD   ") || (text.startsWith("ALB ")) || (text.startsWith("LOAD ALB ")) || (text.length() > 12)) {
        source = sourceCD;
    }
    else if ((text == "CD CHANGER  ") || (text.startsWith("CD "))) {
        source = sourceCDChanger;
        /*if (currentSource != source) {
            text = "USB";
            emit radioTextChanged(text);
        }*/
    }
    else if ((text == "RADIO FM    ")) {
        source = sourceFM;
    }

    if ((currentSource != source) && (source != sourceUnknown)) {
        currentSource = source;
        updateSource(currentSource);
    }

}

void ClioMK3::updateSource(enum AFFA2Source source) {
    switch(source){
        case AFFA2Source::sourceFM : this->info->source->setText("Radio");
            break;
        case AFFA2Source::sourceCD : this->info->source->setText("CD");
            break;
        case AFFA2Source::sourceCDChanger : this->info->source->setText("CD charger");
            break;
        case AFFA2Source::sourceAUX : this->info->source->setText("Bluetooth");
            break;
        case AFFA2Source::sourceUnknown : this->info->source->setText("Unknown");
            break;

    }
}

void ClioMK3::updateMenu(QString text) {
    this->info->menu->setText(text);
    ClioMK3_LOG(info) << "[updateMenu] " << text.toStdString();
}

void ClioMK3::showMenu() {
    this->info->text->setVisible(false);
    this->info->menu->setVisible(true);
    ClioMK3_LOG(info) << "[showMenu]";
}

void ClioMK3::hideMenu() {
    this->info->menu->setText("");
    this->info->menu->setVisible(false);
    this->info->text->setVisible(true);
    ClioMK3_LOG(info) << "[hideMenu]";
}

void ClioMK3::stopBlinkText(){
    ClioMK3_LOG(error)<<"[stopBlink]";
    // Remove All Element
    this->info->listText.clear();
    currentBlinkPosition = 0;
    if(this->info->blink->isVisible()){
        this->info->blink->setVisible(false);
        this->info->text->setVisible(true);
    }
}

void ClioMK3::updateBlinkText(QString text){
    ClioMK3_LOG(error)<<"[updateBlink]";

    if(!QString(text).isNull())
        this->info->listText.append(text);

    // For the first time only
    if(this->info->listText.size()==1 && !QString(text).isNull()){
        ClioMK3_LOG(error)<<"first call";
        // on affiche la zone
        if(!this->info->blink->isVisible()){
            this->info->blink->setVisible(true);
            this->info->text->setVisible(false);
        }
	    //this->info->blink->setText(this->info->listText.at(currentBlinkPosition));
	    //currentBlinkPosition++;
        // Premier appel on le démarre tout de suite
        ClioMK3_LOG(error)<<"set timer";
        QTimer *timer = new QTimer(this);
        connect(timer,SIGNAL(timeout()), this, SLOT(blink()));
	    timer->setSingleShot(true);
        timer->start();
        //QTimer::singleShot(1, &blink);
    }

}

void ClioMK3::blink(){
    ClioMK3_LOG(error)<<"blink";
    if(!this->info->listText.empty()){
        ClioMK3_LOG(error)<<"set new text";
        this->info->blink->setText(this->info->listText.at(currentBlinkPosition));
        if(currentBlinkPosition+1<this->info->listText.size()){
            currentBlinkPosition++;
        }else{
            currentBlinkPosition=0;
        }

        QTimer *timer = new QTimer(this);
        connect(timer,SIGNAL(timeout()), this, SLOT(blink()));
        timer->setSingleShot(true);
        timer->start(800);
        //QTimer::singleShot(500,&blink);
    }
}

void ClioMK3::radioIconsChanged(bool news, bool traffic, bool afrds){
    ClioMK3_LOG(error)<<"iconsChanged";
    ClioMK3_LOG(error)<<"values "<<news<<" "<<traffic<<" "<<afrds;
    this->info->news->setVisible(news);
    this->info->traffic->setVisible(traffic);
    this->info->rds->setVisible(afrds);
    
}

void ClioMK3::modeChanged(enum AFFA2Mode mode){
    switch(mode){
        case AFFA2Mode::UpdateList :
            this->info->updateListMode->setVisible(true);
            this->info->presetMode->setVisible(false);
            this->info->manualMode->setVisible(false);
            break;
        case AFFA2Mode::PresetMode :
            this->info->updateListMode->setVisible(false);
            this->info->presetMode->setVisible(true);
            this->info->manualMode->setVisible(false);
            break;
        case AFFA2Mode::ManualMode :
            this->info->updateListMode->setVisible(false);
            this->info->presetMode->setVisible(false);
            this->info->manualMode->setVisible(true);
            break;
        default:
            this->info->updateListMode->setVisible(false);
            this->info->presetMode->setVisible(false);
            this->info->manualMode->setVisible(false);
            break;
    }
}
