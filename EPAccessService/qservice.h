#ifndef QSERVICE_H
#define QSERVICE_H

#include <QObject>
#include "accesshandler.h"
#include "connector.h"
//#include "tcphandler.h"
#include "ldapcore/qldap.h"

// класс основного сервиса
class QService : public QObject
{
    Q_OBJECT

public:
    QService(QString str);                      // конструктор
    ~QService();                                // деструктор
    void start();                               // запуск службы

signals:

private slots:
    void slotConnected(ldapcore::QLdap*);       // действие при восстановлении соединения
    void slotDisconnected();                    // действие при ошибке соединения

private:
    //TCPhandler *mTCPhandler = nullptr;          // обработчик tcp-запросов
    AccessHandler *mAccessHandler = nullptr;    // обработчик временного доступа
    Connector *mConnector = nullptr;            // устанновщик соединения
    ldapcore::QLdap *mLDAP = nullptr;           // экземпляр LDAP
    bool isConnected = false;                   // флаг соединения с LDAP
};

#endif // QSERVICE_H
