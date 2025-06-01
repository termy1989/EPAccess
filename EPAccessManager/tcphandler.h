#ifndef TCPHANDLER_H
#define TCPHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include <QStandardItemModel>
#include "logindialog.h"

// класс обработки TCP
class TCPhandler : public QObject
{
    Q_OBJECT

public:
    TCPhandler();                                           // конструктор
    ~TCPhandler();                                          // деструктор
    void authOnServer(const Auth&);                         // авторизация на сервере
    void sendAuthReq(const Auth&);                          // отправка запроса авторизации
    void sendUpdStructReq(const QString&);                  // отправка запроса на структуру списка пользователей
    void sendUpdUsersReq();                                 // отправка запроса на список пользователей
    void sendEditAccessReq(const quint8,
                           const QStringList&);             // отправка запроса на установку доступа
    void sendDelAccessReq(const quint8,
                          const QStringList&);              // отправка запроса на отключение доступа
    void sendResetReq(const QStringList&);                  // отправка запроса на сброс пароля

signals:
    void signalAuthSuccess();                               // сигнал с результатами авторизации
    void signalUpdateUserStruct(QString);                   // сигнал о получении структуры таблицы
    void signalUpdateAttributes(QStringList);               // cигнал о получении списка атрибутов
    void signalUpdateUserList(QStandardItemModel*);         // сигнал о получении списка пользователей
    void signalOperationReady(bool);                        // сигнал о завершении сброса пароля
    void signalUpdateProgress(quint16, quint16,
                              QString, QString);            // сигнал о текущем прогрессе выполнения задачи
    void signalConnectError(QString);                       // сигнал об ошибке соединения

public slots:
    void slotDiscardSocket();                               // отключение соединения

private slots:
    void slotInitSocket();                                  // инициализация сокета
    void slotSendRequest(const QString&,
                         const QString&);                   // отправка запроса
    void slotReadSocket();                                  // получение ответа от сервера
    void slotResponseHandler(QByteArray);                   // обработка ответа от сервера

    void slotAuth(const QString&);                          // обработка авторизации
    void slotCreateUserStruct(const QString&);              // создание структуры таблицы
    void slotUpdateUserModel(const QString&);               // запрос на информацию о пользователе
    void slotEditAccess(const QString&);                    // запрос на редактирование доступа
    void slotDelAccess(const QString&);                     // запрос на отключение доступа
    void slotResetPassword();                               // сброс пароля

    void slotDisplayError(QAbstractSocket::SocketError);    // ошибка соединения

private:
    QTcpSocket *mSocket = nullptr;                          // экземпляр ТСР сокета
    Auth mAuth;                                             // структура авторизации
    quint16 mUserCount = 0;                                 // число пользователей
    quint16 mUserIndex = 0;                                 // счетчик пользователей
    QStringList mUserList;                                  // список пользователей для обработки
    QStandardItemModel* mUsersModel = nullptr;              // модель таблицы пользователей
    bool isWaitResponse = false;                            // флаг ожидания ответа на вопрос
};

#endif // TCPHANDLER_H
