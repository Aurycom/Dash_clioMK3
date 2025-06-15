#include "ClioMK3.hpp"

#define DEBUG false

InfoWindow::InfoWindow(Arbiter &arbiter, QWidget *parent) : QWidget(parent)
{
    /*QLabel* test = new QLabel("Test", this);*/
    text = new QLabel("--", this);
    menu = new QLabel("", this);
    blink = new QLabel("", this);
    menu->setVisible(false);
    blink->setVisible(false);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(text);
    layout->addWidget(menu);
    layout->addWidget(blink);
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
    this->info->text->setText(text);
    ClioMK3_LOG(info) << "[updateText] " << text.toStdString();
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
        timer->start(1000);
        //QTimer::singleShot(500,&blink);
    }
}
