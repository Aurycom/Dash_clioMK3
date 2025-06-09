#include "AFFA2emulator.hpp"
#include <QDebug>
#include <QProcess>

AFFA2Emulator::AFFA2Emulator(){}

void AFFA2Emulator::getText(can_frame *frame) {
    static char buf[32];
    static int bufpos = 0;
    static int iconsmask, mode, chan, location;
    static QString longText;
    int ptr, max, idx;
    bool selected, fullscreen;
    char c;
    QString text;

    if (frame->data[0] == 0x70) { /* Enregistrement des fonctionnalités */
        _packetSendReply(frame);
    }
    else if (frame->data[0] == 0x04) { /* Définir des icônes */
        /* TODO!!! */
        qDebug() << "DisplayEmulator: TODO: Set icons";
        //qDebug() << frame->data[3];
        //iconsmask = frame->data[3];
        //mode = frame->data[5];
        //emit displayIconsChanged(iconsmask);
        _packetSendReply(frame);
    }
    else if (frame->data[0] == 0x10) { /* Définir le texte */
        bufpos = 0;
        buf[0] = '\0';

        if (frame->data[1] == 0x1C) { /* Texte + icones */
            iconsmask = frame->data[3];
            mode = frame->data[5];
            ptr = 6;
            emit displayIconsChanged(iconsmask);
        }
        else if (frame->data[1] == 0x19) { /* Texte uniquement */
            ptr = 3;
        }

        chan = frame->data[ptr++] & 7;
        location = frame->data[ptr++];

        for( ; ptr < 8; ptr++) {
            buf[bufpos++] = frame->data[ptr];
        }

        _packetSendReply(frame, false);
    }
    else if(frame->data[0] > 0x20) { /* Suite des données précédentes */
        ptr = 1;
        while(ptr < 8) {
            buf[bufpos] = frame->data[ptr++];
            if (!buf[bufpos])
                break;

            bufpos++;
        }

        if (ptr < 8) { /* Plus de données */
            _packetSendReply(frame, true);

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