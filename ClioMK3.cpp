#include "ClioMK3.hpp"

#define DEBUG false

InfoWindow::InfoWindow(Arbiter &arbiter, QWidget *parent) : QWidget(parent)
{
    /*QLabel* test = new QLabel("Test", this);*/
    text = new QLabel("--", this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(text);
}

ClioMK3::~ClioMK3()
{
    /*if (this->vehicle)
        delete this->vehicle;*/
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

void AFFA2EmulatorDisplay::getText(QByteArray frame) {
    static char buf[32];
    static int bufpos = 0;
    static int iconsmask, mode, chan, location;
    static QString longText;
    int ptr, max, idx;
    bool selected, fullscreen;
    char c;
    QString text;

    if (frame.at(0) == 0x70) { /* Enregistrement des fonctionnalités */
        //_packetSendReply(frame);
    }
    else if (frame.at(0) == 0x04) { /* Définir des icônes */
        /* TODO!!! */
        qDebug() << "DisplayEmulator: TODO: Set icons";
        //qDebug() << frame.at(3);
        //iconsmask = frame.at(3);
        //mode = frame.at(5);
        //emit displayIconsChanged(iconsmask);
        //_packetSendReply(frame);
    }
    else if (frame.at(0) == 0x10) { /* Définir le texte */
        bufpos = 0;
        buf[0] = '\0';

        if (frame.at(1) == 0x1C) { /* Texte + icones */
            iconsmask = frame.at(3);
            mode = frame.at(5);
            ptr = 6;
            //emit displayIconsChanged(iconsmask);
        }
        else if (frame.at(1) == 0x19) { /* Texte uniquement */
            ptr = 3;
        }

        chan = frame.at(ptr++) & 7;
        location = frame.at(ptr++);

        for( ; ptr < 8; ptr++) {
            buf[bufpos++] = frame.at(ptr);
        }

        //_packetSendReply(frame, false);
    }
    else if(frame.at(0) > 0x20) { /* Suite des données précédentes */
        ptr = 1;
        while(ptr < 8) {
            buf[bufpos] = frame.at(ptr++);
            if (!buf[bufpos])
                break;

            bufpos++;
        }

        if (ptr < 8) { /* Plus de données */
            //_packetSendReply(frame, true);

            /* Traitement des données */
            for(ptr = 0; ptr < 12; ptr++) {
                c = char(buf[9 + ptr] & 0x7F);
                switch(c) {
                    case 0x07: text += QString::fromUtf8("⇧"); break;
                    case 0x08: text += QString::fromUtf8("⇩"); break;
                    case 0x09: text += QString::fromUtf8("⇨"); break;
                    case 0x0A: text += QString::fromUtf8("⇦"); break;
                    default: text += c;
                }
            }

            //qDebug() << " loc =" << location << " buf = " << buf << "text =" << text;

            max = (location >> 5) & 7;
            idx = (location >> 2) & 7;
            selected = ((location & 0x01) == 0x01);
            fullscreen = ((location & 0x02) == 0x02);

            if ((max > 0) && (fullscreen) && (!selected)) { /* Texte clignotant (np. NEWS -> RMF FM -> NEWS...) */

            }
            else {

            }

            /*if ((max > 0) && (!fullscreen)) { /* Mode menu *
                if (!_menuVisible) {
                    emit displayMenuShow(max + 1);
                    _menuVisible = true;
                }

                emit displayMenuItemUpdate(idx, text, selected);
                return;
            }
            else { /* Menu invisible *
                if (_menuVisible) {
                    emit displayMenuHide();
                    _menuVisible = false;
                }
            }*/

            if ((max == 0) && (selected)) { /* Texte brut */
                emit displayTextChanged(text);
            }
            else if ((fullscreen) && (selected)) { /* Mode plein écran */
                if (idx == 0) {
                    longText.clear();
                }

                longText += text.left(8);

                if (idx == max) {
                    emit displayTextChanged(longText);
                }
            }
        }
        else {
            //_packetSendReply(frame, false);
        }
    }
}
