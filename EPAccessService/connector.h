#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QObject>
#include <QTimer>
#include <QString>
#include "ldapcore/qldap.h"

// класс установки LDAP-соединения
class Connector : public QObject
{
    Q_OBJECT

public:
    Connector(QString);                             // конструктор
    ~Connector();                                   // деструктор
    void start();                                   // старт коннектора
    void stop();                                    // остановка коннектора

private slots:
    void slotConnect();                             // попытка подключения к LDAP

signals:
    void signalConnected(ldapcore::QLdap*);         // сигнал об установке соединения
    void signalTimerStart(int);                     // сигнал запуска таймера
    void signalTimerStop();                         // сигнал остановки таймера

private:
    QTimer *mTimer = nullptr;                       // таймер повторной установки соединения
    ldapcore::QLdap *mLDAP = nullptr;               // экземпляр LDAP
    QString mFileName = "";                         // конфиг-файл
};

#endif // CONNECTOR_H
