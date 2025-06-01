#ifndef ACCESSHANDLER_H
#define ACCESSHANDLER_H

#include <QThread>
#include <QTimer>
#include "ldapcore/qldap.h"
#include "ldapcore/qldapuser.h"

// класс обработки доступа в потоке
class AccessHandler : public QThread
{
    Q_OBJECT
public:
    void run();                                             // запуск потока
    void work(ldapcore::QLdap*);                            // старт обработчика
    void stop();                                            // остановка таймера
    int clearAccess(const ldapcore::QLdapEntryList*);       // очистка доступа по истёкшей дате
    bool addAttrGroup(const ldapcore::QLdapUser*,
                      const int, const QString);            // добавление атрибута и членства в группе
    bool deleteAttrGroup(const ldapcore::QLdapUser*,
                         const int);                        // удаление атрибута и членства в группе

signals:
    void stopTimer();                                       // сигнал остановки таймера
    void signalConnectError();                              // сигнал об ошибке соединения

private slots:
    void slotCheckAccess();                                 // проверка доступа

private:
    ldapcore::QLdap *mLDAP = nullptr;                       // экземпляр LDAP
    bool isBusy = false;                                    // флаг занятости обработчика
    bool isRetryCheckingAccess = false;                     // флаг повторной попытки провести проверку доступа
};

#endif // ACCESSHANDLER_H
