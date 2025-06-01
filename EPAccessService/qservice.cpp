#include "qservice.h"

// конструктор
QService::QService(const QString &filename) {

    // инициализация коннектора
    mConnector = new Connector(filename);
    connect(mConnector, SIGNAL(signalConnected(ldapcore::QLdap*)),
            this, SLOT(slotConnected(ldapcore::QLdap*)));

    // инициализация обработчика доступа
    mHandler = new Handler();
    connect(mHandler, SIGNAL(signalConnectError()),
                this, SLOT(slotDisconnected()));

    // старт коннектора
    mConnector->start();
}

// деструктор
QService::~QService() {
    delete mHandler;
    delete mConnector;
}

// соединение yстановлено
void QService::slotConnected(ldapcore::QLdap* ldap) {
    isConnected = true;                                     // флаг активного подключения к LDAP
    mLDAP = ldap;                                           // инициализация экземпляра LDAP
    mConnector->stop();                                     // прекращение работы коннектора
    mHandler->start(mLDAP);                           // старт обработчика доступа
}

// соединение потеряно
void QService::slotDisconnected() {
    isConnected = false;                                    // флаг отсутствия подключения к LDAP
    mHandler->stop();                                 // остановка работы обработчика доступа
    delete mLDAP;                                           // уничтожение экземпляра LDAP
    mConnector->start();                                    // старт коннектора для восстановления соединения с LDAP
}
