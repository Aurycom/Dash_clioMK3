#include <QObject>
#include <QThread>

/* Icônes sur l'écran */
#define DISPLAY_ICON_NO_NEWS       (1 << 0)
#define DISPLAY_ICON_NEWS_ARROW    (1 << 1)
#define DISPLAY_ICON_NO_TRAFFIC    (1 << 2)
#define DISPLAY_ICON_TRAFFIC_ARROW (1 << 3)
#define DISPLAY_ICON_NO_AFRDS      (1 << 4)
#define DISPLAY_ICON_AFRDS_ARROW   (1 << 5)
#define DISPLAY_ICON_NO_MODE       (1 << 6)
#define DISPLAY_ICON_MODE_NONE     0xFF

#define DISPLAY_KEY_LOAD           0x0000 /* Celui en bas du pilote ;) */
#define DISPLAY_KEY_SRC_RIGHT      0x0001
#define DISPLAY_KEY_SRC_LEFT       0x0002
#define DISPLAY_KEY_VOLUME_UP      0x0003
#define DISPLAY_KEY_VOLUME_DOWN    0x0004
#define DISPLAY_KEY_PAUSE          0x0005
#define DISPLAY_KEY_ROLL_UP        0x0101
#define DISPLAY_KEY_ROLL_DOWN      0x0141
#define DISPLAY_KEY_HOLD_MASK      (0x80 | 0x40)

#define CAN_ID_SYNC_DISPLAY        0x3CF  /* Pack de synchronisation, affichage -> HU */
#define CAN_ID_SYNC_HU             0x3DF  /* Forfait de synchronisation, HU -> afficher */
#define CAN_ID_DISPLAY_CONTROL     0x1B1  /* Configuration de l'affichage, HU -> afficher */
#define CAN_ID_DISPLAY_STATUS      0x1C1  /* Modifier l'état d'affichage, afficher -> HU */
#define CAN_ID_SET_TEXT            0x121  /* Réglage du texte à l'écran, HU -> afficher */
#define CAN_ID_KEY_PRESSED         0x0A9  /* Commandes au volant, afficher -> HU */

#define CAN_ID_REPLY_FLAG          0x400  /* Indicateur défini pour la réponse */

class AFFA2Emulator : public QObject {
    
    Q_OBJECT
    
    private:

    public:
        AFFA2Emulator() {};
        ~AFFA2Emulator();

}