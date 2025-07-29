#include "ClioMK3.hpp"

#define DEBUG false

InfoWindow::InfoWindow(Arbiter &arbiter, QWidget *parent) : QWidget(parent)
{

    /*Dialog *dialog = new Dialog(this->arbiter, true, this->window());
    dialog->set_body(menu);*/

    QVBoxLayout *globalLayout = new QVBoxLayout(this);
    globalLayout->setContentsMargins(100,0,100,0);
    globalLayout->setAlignment(Qt::AlignVCenter);

    QHBoxLayout *layoutSource = new QHBoxLayout(this);
    source = new QLabel("", this);
    layoutSource->addWidget(source);
    globalLayout->addLayout(layoutSource);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0,50,0,50);
    text = new QLabel("--", this);
    text->setObjectName("radioMainText");
    menu = new QLabel("", this);
    menu->setObjectName("menu");
    blink = new QLabel("", this);
    //menu->setVisible(false);

	nbMenuRow=0;
    currentItemObject="";

    /*gridMenu->addWidget(menu, 0, Qt::AlignLeft);
    gridMenu->addStretch(); */

    dialog = new DialogAutoResize(arbiter, true, arbiter.window());
    dialog->set_title("Menu");
    int margin = std::ceil(48 * arbiter.layout().scale) * 2;
    //dialog->setFixedWidth(dialog->parentWidget()->width() - ( 2 * margin ));
    //dialog->setFixedHeight(dialog->parentWidget()->height());
    //dialog->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    //dialog->setFixedWidth();
    //dialog->setFixedHeight();
    //dialog->setStyleSheet("min-height:25vh;min-width:50vw;max-height:90vh;max-width:90vw");

	//QVBoxLayout *testLayout = new QVBoxLayout(dialog);
        //testLayout->addStretch();
	//testLayout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
	//testLayout->setContentsMargins(0,0,0,0);
	mainDalogWidget = new QWidget(dialog);
	//testLayout->addWidget(mainDalogWidget);
	gridMenu = new QGridLayout(mainDalogWidget);
	gridMenu->setContentsMargins(0,0,0,0);
	//gridMenu->setSizeConstraint(QLayout::SetFixedSize);
	//gridMenu->addStretch();
	gridMenu->setSpacing(0);
	gridMenu->activate();
	gridMenu->setVerticalSpacing(0);
	//testLayout->addLayout(gridMenu);

    dialog->set_body(mainDalogWidget);
    //dialog->setLayout(testLayout);
    dialog->hide();

    blink->setVisible(false);
    blink->setObjectName("radioMainText");
    layout->addWidget(text);
    //layout->addWidget(menu);
    layout->addWidget(blink);
    globalLayout->addLayout(layout);

    QHBoxLayout *layoutRadioItems = new QHBoxLayout(this);
    layoutRadioItems->setSpacing(20);
    layoutRadioItems->setAlignment(Qt::AlignLeft);
    layoutRadioItems->setContentsMargins(0,0,0,0);
    layoutRadioItems->setObjectName("radioItems");
    rds = new QLabel("AF-RDS", this);
    rds->setObjectName("tag");
    rds->setVisible(false);
    news = new QLabel("iNews", this);
    news->setObjectName("tag");
    news->setVisible(false);
    traffic = new QLabel("iTraffic", this);
    traffic->setObjectName("tag");
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

ClioMK3::ClioMK3() : QObject(){
    ClioMK3_LOG(info)<<"constructor";
    if(this->arbiter){
        this->setParent(this->arbiter);
        ClioMK3_LOG(info)<< "parent init";
    }else{
        ClioMK3_LOG(info)<< "NO PARENT";
    }
}

ClioMK3::~ClioMK3()
{
    if (this->vehicle)
        delete this->vehicle;

    this->currentBlinkPosition=0;
    ClioMK3_LOG(info)<<"stop_all";
    std::cout << "Destructeur appelé" << std::endl;
    /*if(this->multiGpioMonitor)
        delete this->multiGpioMonitor;*/
}

bool ClioMK3::init(ICANBus* canbus){
    if (this->arbiter) {
        this->setParent(this->arbiter);
	for(auto page : this->arbiter->layout().pages()){
            ClioMK3_LOG(info)<<page->name().toStdString();
            if(page->name().toStdString() == "Vehicle" ){
                //VehiclePage* vp = qobject_cast<VehiclePage*>(page);
                ClioMK3_LOG(info)<<"Found the page";
                //page->removeTab(0);
                this->arbiter->set_curr_page(page);
            }
        }

	//Config::get_instance()->openauto_config->setWifiSSID("AndroidClioAuto");
	//Config::get_instance()->openauto_config->setWifiPassword("@urycom049");
	//Config::get_instance()->openauto_config->setWifiMAC("e4:5f:01:e6:e9:0c");
	//ClioMK3_LOG(info)<<Config::get_instance()->openauto_config->getWifiSSID();
        this->aa_handler = this->arbiter->android_auto().handler;
		/*this->vehicle = new Vehicle(*this->arbiter);
        this->vehicle->setObjectName("Clio 3 Phase 1");*/

        this->info = new InfoWindow(*this->arbiter);
        this->info->setObjectName("Info");

	//this->info->dialog->setParent(this->window());

        this->display = new AFFA2EmulatorDisplay();
	canbus->registerFrameHandler(0x121, [this](QByteArray payload){this->display->getText(payload);});

        connect(this->display, SIGNAL(displayTextChanged(QString)), this, SLOT(updateText(QString)));
        connect(this->display, SIGNAL(displayMenuShow()), this, SLOT(showMenu()));
        connect(this->display, SIGNAL(displayMenuHide()), this, SLOT(hideMenu()));

        connect(this->display, SIGNAL(addBlinkText(QString)), this, SLOT(updateBlinkText(QString)));
        connect(this->display, SIGNAL(stopBlinkText()), this, SLOT(stopBlinkText()));
        connect(this->display, SIGNAL(radioIconsChanged(bool, bool, bool)), this, SLOT(radioIconsChanged(bool, bool, bool)));
        connect(this->display, SIGNAL(modeChanged(enum AFFA2Mode)), this, SLOT(modeChanged(enum AFFA2Mode)));

        ClioMK3_LOG(info)<<"init success";



	this->multiGpioMonitor = new MultiGpioMonitorQt(this);
	multiGpioMonitor->add("gpiochip0", 13, [this](unsigned int offset, bool val) {
            ClioMK3_LOG(info)<< "GPIO is now " << (val ? "HIGH" : "LOW");
	    emit light_status_changed(val);
        });

	try {
        	this->multiGpioMonitor->start_all();
	        //ClioMK3_LOG(info) << "Monitoring GPIO 13...";

    	} catch (const std::exception& e) {
        	ClioMK3_LOG(error) << "Exception: " << e.what();
	}





        return true;
    }else{
    	ClioMK3_LOG(error)<<"Failed to get arbiter";
        return false;
    }

}

void ClioMK3::signalHandler(int) {
    keepRunning = false;
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

void ClioMK3::updateMenu(QString text, bool selected) {

    if(selected && this->info->nbMenuRow>0){
        // Replace part of old item
        QLabel* oldPrev = this->info->mainDalogWidget->findChild<QLabel*>("prev_"+this->info->currentItemObject, Qt::FindDirectChildrenOnly);
        oldPrev->setText("");
        QLabel* oldNext = this->info->mainDalogWidget->findChild<QLabel*>("next_"+this->info->currentItemObject, Qt::FindDirectChildrenOnly);
        oldNext->setText("");
        
        //new Element
        this->info->currentItemObject = generateObjectName(text);
        QLabel* newPrev = this->info->mainDalogWidget->findChild<QLabel*>("prev_"+this->info->currentItemObject, Qt::FindDirectChildrenOnly);
        newPrev->setText(QString::fromUtf8("⇦"));
	newPrev->updateGeometry();
        QLabel* newItem = this->info->mainDalogWidget->findChild<QLabel*>(this->info->currentItemObject, Qt::FindDirectChildrenOnly);
        newItem->setText(text);
	newItem->updateGeometry();
        QLabel* newNext = this->info->mainDalogWidget->findChild<QLabel*>("next_"+this->info->currentItemObject, Qt::FindDirectChildrenOnly);
        newNext->setText(QString::fromUtf8("⇨"));
	newNext->updateGeometry();

    }else{
        QString itemObjectName = generateObjectName(text);
        if(selected){
            this->info->currentItemObject = itemObjectName;
        }

        int currentRow = this->info->nbMenuRow;

        QLabel* prevItem = new QLabel(this->info->mainDalogWidget);
	//prevItem->setFixedHeight(20);
	//prevItem->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	//prevItem->setStyleSheet("border: 1px solid green;");
        prevItem->setObjectName("prev_"+itemObjectName);
        if(selected){
            prevItem->setText(QString::fromUtf8("⇦"));
        }else{
            prevItem->setText("");
        }
        this->info->gridMenu->addWidget(prevItem,currentRow,0);
	//this->info->gridMenu->setRowStretch(currentRow,1);

        QLabel* item = new QLabel(this->info->mainDalogWidget);
	//item->setFixedHeight(20);
	//item->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	
        //item->setStyleSheet("border: 1px solid green;");
        item->setObjectName(itemObjectName);
        item->setText(text);
        this->info->gridMenu->addWidget(item,currentRow,1);

        QLabel* nextItem = new QLabel(this->info->mainDalogWidget);
	//nextItem->setFixedHeight(20);
	//nextItem->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        //nextItem->setStyleSheet("border: 1px solid green;");
        if(selected){
            nextItem->setText(QString::fromUtf8("⇨"));
        }else{
            nextItem->setText("");
        }
        nextItem->setObjectName("next_"+itemObjectName);
        this->info->gridMenu->addWidget(nextItem,currentRow,2);

        this->info->nbMenuRow++;

	// update dialog size
	/*this->info->gridMenu->update();
	this->info->dialog->update();*/
	this->info->gridMenu->invalidate();
	this->info->gridMenu->activate();
	this->info->gridMenu->update();
	this->info->mainDalogWidget->adjustSize();
	this->info->mainDalogWidget->updateGeometry();
	this->info->mainDalogWidget->update();
	//this->info->dialog->setMinimumSize(this->info->mainDalogWidget->size());
        this->info->dialog->resize();
	/*this->info->dialog->updateGeometry();
	this->info->dialog->update();
	this->info->dialog->hide();
	this->info->dialog->show();*/
	//this->info->adjustSize();
	ClioMK3_LOG(info) << "[updateMenu] "<<this->info->gridMenu->sizeHint().height();
	//this->info->dialog->setFixedHeight(this->info->gridMenu->sizeHint().height());142
	//this->info->dialog->setFixedWidth(this->info->gridMenu->sizeHint().width());
	//this->info->dialog->setMinimumSize(this->info->dialog->size());
    }

    
    
    /*
    if(selected){
        // Always first option
        // but sometimes, the only one
	if(this->info->menu->text().size()>0){
		if((text.contains("ON") || text.contains("OFF")) && !this->info->menu->text().contains(text)){
			ClioMK3_LOG(info) << "[updateMenu] contains ON or OFF";
			text = text.trimmed();
			QString oldText = text;
			if(text.contains("OFF"))
				oldText.replace("OFF","ON");
			if(text.contains("ON"))
                        	oldText.replace("ON","OFF");
			ClioMK3_LOG(info) << "[updateMenu] old '" << oldText.toStdString() <<"'";
			ClioMK3_LOG(info) << "[updateMenu] new '" << text.toStdString() << "'";
			ClioMK3_LOG(info) << "[updateMenu] " << this->info->menu->text().indexOf(oldText);
			if(this->info->menu->text().contains(oldText)){
				this->info->menu->setText(this->info->menu->text().replace(oldText,text));
			}else{
				ClioMK3_LOG(info) << "[updateMenu] can't find old text";
			}
		}else{
			this->info->menu->setText(this->info->menu->text().replace(QString::fromUtf8("⇦"),"     "));
			this->info->menu->setText(this->info->menu->text().replace(QString::fromUtf8("⇨"),""));
			this->info->menu->setText(this->info->menu->text().replace("     "+text,QString::fromUtf8("⇦")+text+QString::fromUtf8("⇨")));
		}
		return;
	}
	//this->info->menu->setText(this->info->menu->text()+QString::fromUtf8("⇦")+" ");
	text = QString::fromUtf8("⇦")+text+QString::fromUtf8("⇨");
    }else{
        //this->info->menu->setText(this->info->menu->text()+"   ");
	text = "     "+text;
    }
    if(this->info->menu->text().size()>0){
        this->info->menu->setText(this->info->menu->text()+"\r\n");
    }
    this->info->menu->setText(this->info->menu->text()+text);
    
    this->info->menu->adjustSize();
    ClioMK3_LOG(info) << "[updateMenu] " << text.toStdString();*/
}

QString ClioMK3::generateObjectName(QString text) {
    text = text.replace("ON","");
    text = text.replace("OFF","");
    text = text.trimmed();
    std::string objectName = text.toStdString();
    std::replace_if(objectName.begin(), objectName.end(), ::isdigit, ' ');
    std::replace_if(objectName.begin(), objectName.end(), ::ispunct, ' ');
    return QString::fromStdString(objectName);;
}

void ClioMK3::showMenu() {
    //this->info->text->setVisible(false);
this->info->gridMenu->activate();
//this->info->dialog->activate();
    this->info->dialog->show();
    ClioMK3_LOG(info) << "[showMenu]";
}

void ClioMK3::hideMenu() {
    //this->info->menu->setText("");

    clearLayout(this->info->gridMenu,true);
	ClioMK3_LOG(info) << "[taille menu] "<<this->info->gridMenu->rowCount();

    this->info->dialog->hide();
//	this->info->dialog->invalidate();
	this->info->gridMenu->invalidate();
    //this->info->text->setVisible(true);
    ClioMK3_LOG(info) << "[hideMenu]";
}

void ClioMK3::clearLayout(QLayout* layout, bool deleteWidgets)
{
    while (QLayoutItem* item = layout->takeAt(0))
    {
        if (deleteWidgets)
        {
            if (QWidget* widget = item->widget())
                widget->deleteLater();
        }
        if (QLayout* childLayout = item->layout())
            clearLayout(childLayout, deleteWidgets);
        delete item;
    }
	this->info->nbMenuRow=0;
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
    if(afrds && currentSource != AFFA2Source::sourceFM) {
        currentSource = AFFA2Source::sourceFM;
        updateSource(currentSource);
    }
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

void ClioMK3::light_status_changed(bool isHigh){
	if(isHigh){
                //headlights are ON - turn to dark mode
                if(this->arbiter->theme().mode == Session::Theme::Light){
                	this->arbiter->set_mode(Session::Theme::Dark);
        	}
        }else{
                //headlights are off or not fully on (i.e. sidelights only) - make sure is light mode
                if(this->arbiter->theme().mode == Session::Theme::Dark){
                	this->arbiter->set_mode(Session::Theme::Light);
        	}
	}
}
