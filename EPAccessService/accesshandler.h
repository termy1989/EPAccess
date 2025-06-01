#ifndef ACCESSHANDLER_H
#define ACCESSHANDLER_H

#include <QTimer>
#include <QDateTime>
#include <QObject>
#include "ldapcore/qldap.h"
#include "ldapcore/qldapuser.h"
#include "tcphandler.h"

// класс контроля доступа
class AccessHandler : public QObject
{
    Q_OBJECT

public:
    AccessHandler();                                        // конструктор
    ~AccessHandler();                                       // деструктор
    void start(ldapcore::QLdap*);                           // включение обработчика
    void stop();                                            // отключение обработчика
    int clearAccess(const ldapcore::QLdapEntryList*);       // очистка доступа по истёкшей дате
    bool addAttrGroup(const ldapcore::QLdapUser*,
                      const int, const QString);            // добавление атрибута и членства в группе
    bool deleteAttrGroup(const ldapcore::QLdapUser*,
                         const int);                        // удаление атрибута и членства в группе
    int searchUsers(const QString);                         // поиск пользователя по указанному условию

private slots:
    void slotCheckAccess();                                 // слот проверки доступа по таймеру
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

signals:
    void signalConnectError();                              // сигнал об ошибке соединения
    void signalTimerStart(int);                             // сигнал запуска таймера
    void signalTimerStop();                                 // сигнал остановки таймера

private:
    ldapcore::QLdap *mLDAP = nullptr;                       // экземпляр LDAP
    ldapcore::QLdapEntryList mUserList;                     // список пользователей
    QTimer *mTimer = nullptr;                               // таймер проверки доступа
    TCPhandler *mTCPhandler = nullptr;                      // обработчик запросов ТСР
    int mUserIndex = 0;                                     // счетчик пользователей
    bool isRetryCheckingAccess = false;                     // флаг повторной попытки провести проверку доступа
};

#endif // ACCESSHANDLER_H
