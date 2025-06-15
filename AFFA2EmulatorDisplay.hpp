#pragma once 

#include <QString>
#include <iostream>
#include <QWidget>
#include <stdlib.h>
#include <QByteArray>
#include <boost/log/trivial.hpp>

#define AFFA2_LOG(severity) BOOST_LOG_TRIVIAL(severity) << "[AFFA2] "

class AFFA2EmulatorDisplay : public QObject
{
    
    Q_OBJECT
    
    private:
        bool _menuVisible;
    public:
        AFFA2EmulatorDisplay() {_menuVisible=false;};
        //~AFFA2Emulator();
        void getText(QByteArray payload);
    signals:
        void displayTextChanged(QString text);
        void displayMenuItemUpdate(QString text);
        void displayMenuShow();
        void displayMenuHide();
        void addBlinkText(QString text);
        void stopBlinkText();

};