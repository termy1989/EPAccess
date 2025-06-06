#ifndef ACCESSHANDLER_H
#define ACCESSHANDLER_H

#include <QTimer>
#include <QDateTime>
#include <QObject>
#include "ldapcore/qldap.h"
#include "ldapcore/qldapuser.h"
#include "tcphandler.h"

// класс контроля доступа по таймеру
class AccessHandler : public QObject
{
    Q_OBJECT

public:
    AccessHandler();                                // конструктор
    ~AccessHandler();                               // деструктор
    void start(ldapcore::QLdap*);                   // включение обработчика
    void stop();                                    // отключение обработчика
    int clearAccess(ldapcore::QLdapEntryList*);     // очистка доступа по истёкшей дате
    bool deleteAttrGroup(ldapcore::QLdapUser*,
                         const QString&,
                         const QString&,
                         const int, QStringList);   // удаление атрибута и членства в группе
    int searchUsers(QString);                       //

private slots:
    void slotAuth(QTcpSocket*, QString);            //
    void slotUpdStruct(QTcpSocket*, QString);       //

    void slotStructToStr(QTcpSocket*);              //
    void slotUpdUsers(QTcpSocket*);                 //
    //void
    //void
    void slotResetPwd(QTcpSocket*, QString);        //
    void slotResetSingle(QTcpSocket*);


    void slotCheckAccess();                         // слот проверки доступа по таймеру

signals:
    void signalConnectError();                      // сигнал об ошибке соединения
    void signalTimerStart(int);                     // сигнал запуска таймера
    void signalTimerStop();                         // сигнал остановки таймера

private:
    QTimer *mTimer = nullptr;                       // таймер проверки доступа
    ldapcore::QLdap *mLDAP = nullptr;               // экземпляр LDAP
    ldapcore::QLdapEntryList mUserList;                         // список пользователей
    int mUserIndex = 0;                                         // счетчик пользователей
    TCPhandler *mTCPhandler = nullptr;
    bool isRetryCheckingAccess = false;             // флаг повторной попытки провести проверку доступа
};

#endif // ACCESSHANDLER_H
