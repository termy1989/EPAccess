#include "qservice.h"

// конструктор
QService::QService(QString filename) {

    // инициализация коннектора
    mConnector = new Connector(filename);
    connect(mConnector, SIGNAL(signalConnected(ldapcore::QLdap*)),
            this, SLOT(slotConnected(ldapcore::QLdap*)));

    // инициализация обработчика доступа
    mAccessHandler = new AccessHandler();
    connect(mAccessHandler, SIGNAL(signalConnectError()),
                this, SLOT(slotDisconnected()));

    // старт коннектора
    mConnector->start();
}

// деструктор
QService::~QService() {
    delete mAccessHandler;
    //delete mTCPhandler;
    delete mConnector;
}

// соединение yстановлено
void QService::slotConnected(ldapcore::QLdap* ldap) {
    isConnected = true;                                     // флаг активного подключения к LDAP
    mLDAP = ldap;                                           // инициализация экземпляра LDAP
    mConnector->stop();                                     // прекращение работы коннектора
    mAccessHandler->start(mLDAP);                           // старт обработчика доступа
    /*mTCPhandler = new TCPhandler(mLDAP);                    // инициализация обработчика TCP
    connect(mTCPhandler, SIGNAL(signalConnectError()),
                        this, SLOT(slotDisconnected()));
    if (!mTCPhandler->getReady())                           // проверка готовности обработчика TCP
        slotDisconnected();*/
}

// соединение потеряно
void QService::slotDisconnected() {
    isConnected = false;                                    // флаг отсутствия подключения к LDAP
    mAccessHandler->stop();                                 // остановка работы обработчика доступа
    /*if (mTCPhandler != nullptr)                             // уничтожение обработчика ТСР
        delete mTCPhandler;*/
    delete mLDAP;                                           // уничтожение экземпляра LDAP
    mConnector->start();                                    // старт коннектора для восстановления соединения с LDAP
}
