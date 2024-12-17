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
    void authOnServer(Auth);                                // авторизация на сервере
    void sendAuthReq(Auth);                                 // отправка запроса авторизации
    void sendUpdStructReq(QString);                         // отправка запроса на структуру списка пользователей
    void sendUpdUsersReq();                                 // отправка запроса на список пользователей
    void sendEditAccessReq(int, QStringList);               // отправка запроса на установку доступа
    void sendDelAccessReq(int, QStringList);                //
    void sendResetReq(QStringList);                         // отправка запроса на сброс пароля
   /* void sendErrorReq(QString);                             //
    void sendTestReq(QString);*/                              //

signals:
    void signalAuthSuccess();                               // сигнал с результатами авторизации
    void signalUpdateUserStruct(QString);                   // сигнал о получении структуры таблицы
    void signalUpdateAttributes(QStringList);               // cигнал о получении списка атрибутов
    void signalUpdateUserList(QStandardItemModel*);         // сигнал о получении списка пользователей
    void signalOperationReady(bool);                        // сигнал о завершении сброса пароля
    void signalUpdateProgress(int, int, QString, QString);  // сигнал о текущем прогрессе выполнения задачи
    void signalConnectError(QString);                       // сигнал об ошибке соединения

public slots:
    void slotDiscardSocket();                               // отключение соединения

private slots:
    void slotInitSocket();                                  // инициализация сокета
    void slotSendRequest(QString, QString);                 // отправка запроса
    void slotReadSocket();                                  // получение ответа от сервера
    void slotResponseHandler(QByteArray);                   // обработка ответа от сервера

    void slotAuth(QString);                                 // обработка авторизации
    void slotCreateUserStruct(QString);                     // создание структуры таблицы
    void slotUpdateUserModel(QString);                      // добавление пользователя к списку
    void slotEditAccess(QString);
    void slotDelAccess(QString);
    void slotResetPassword();                               // сброс пароля

    void slotDisplayError(QAbstractSocket::SocketError);    // ошибка соединения

private:
    QTcpSocket *mSocket = nullptr;                          // экземпляр ТСР сокета
    Auth mAuth;                                             // структура авторизации
    int mUserCount = 0;                                     // число пользователей
    int mUserIndex = 0;                                     // счетчик пользователей
    QStringList mUserList;                                  // список пользователей для обработки
    QStandardItemModel* mUsersModel = nullptr;              // модель таблицы пользователей
    bool isWaitResponse = false;                            //
};

#endif // TCPHANDLER_H
