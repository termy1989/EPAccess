#ifndef HANDLER_H
#define HANDLER_H

#include <QTimer>
#include <QDateTime>
#include <QObject>
#include "ldapcore/qldap.h"
#include "ldapcore/qldapuser.h"
#include "tcphandler.h"
#include "accesshandler.h"

// класс контроля доступа
class Handler : public QObject
{
    Q_OBJECT

public:
    Handler();                                              // конструктор
    ~Handler();                                             // деструктор
    void start(ldapcore::QLdap*);                           // включение обработчика
    void stop();                                            // отключение обработчика
    int searchUsers(const QString);                         // поиск пользователя по указанному условию

private slots:
    void slotAuth(QTcpSocket*, const QString&);             // отправка ответа - результаты авторизации
    void slotUpdStruct(QTcpSocket*, const QString&);        // отправка ответа - структура таблицы
    void slotStructToStr(QTcpSocket*);                      // преобразование структуры таблицы в строку
    void slotUpdUsers(QTcpSocket*);                         // отправка ответа - информация об очередном пользователе
    void slotEditAccess(QTcpSocket*, const QString&);       // редактирование доступа для пользователя
    void slotEditAccessSingle(QTcpSocket*, const QString&); // отправка ответа - установка доступа
    void slotDelAccess(QTcpSocket*, const QString&);        // отключение доступа у пользователя
    void slotDelAccessSingle(QTcpSocket*, const QString&);  // отправка ответа - отключение доступа
    void slotResetPwd(QTcpSocket*, const QString&);         // сброс пароля с установкой пароля по умолчанию
    void slotResetPwdSingle(QTcpSocket*);                   // отправка ответа - сброс пароля
    void slotSendError();

signals:
    void signalConnectError();                              // сигнал об ошибке соединения

private:
    ldapcore::QLdap *mLDAP = nullptr;                       // экземпляр LDAP
    ldapcore::QLdapEntryList mUserList;                     // список пользователей
    QTimer mTimer;                                          // таймер проверки доступа
    TCPhandler *mTCPhandler = nullptr;                      // обработчик запросов ТСР
    AccessHandler mAccessHandler;                           // обработчик доступа
    int mUserIndex = 0;                                     // счетчик пользователей
};

#endif // HANDLER_H
