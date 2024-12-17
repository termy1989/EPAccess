#include "connector.h"

// конструктор
Connector::Connector(QString filename) {

    // инициализация таймера
    mFileName = filename;
    mTimer = new QTimer(nullptr);
    connect(this, SIGNAL(signalTimerStart(int)), mTimer, SLOT(start(int)));
    connect(this, SIGNAL(signalTimerStop()), mTimer, SLOT(stop()));
    connect(mTimer, SIGNAL(timeout()), this, SLOT(slotConnect()));
}

// деструктор
Connector::~Connector() {
    delete mTimer;
    delete mLDAP;
}

// включение коннектора
void Connector::start() {
    emit signalTimerStart(3000);
}

// отключение коннектора
void Connector::stop() {
    emit signalTimerStop();
}

// попытка установить соединение
void Connector::slotConnect() {
    mLDAP = new ldapcore::QLdap(mFileName);

    // успешное соединение
    if (mLDAP->connect() == LDAP_SUCCESS)
        emit signalConnected(mLDAP);

    // неудачное соединение
    else {
        mLDAP->close();
        delete mLDAP;
    }
}
