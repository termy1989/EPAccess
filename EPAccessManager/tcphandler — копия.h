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
    TCPhandler(Auth);                                       // конструктор
    ~TCPhandler();                                          // деструктор
    void authOnServer();
    void sendAuthReq();                                     //
    void sendUpdStructReq(QString);                         // отправка запроса на структуру списка пользователей
    void sendUpdUsersReq();                          // отправка запроса на список пользователей
    void sendResetReq(QString);                             // отправка запроса на сброс пароля
    void sendAccessReq(QString);                            // отправка запроса на установку доступа
    void sendTestReq(QString);                            //

signals:
    void signalUpdateUserStruct(QString);                   // сигнал о получении структуры таблицы
    void signalUpdateUserList(QStandardItemModel*);
    void signalConnectError(QString);                       // сигнал об ошибке соединения

private slots:
    void slotSendRequest(QString, QString);                 // отправка запроса
    void slotReadSocket();                                  //
    void slotResponseHandler(QByteArray);
    void slotCreateUserStruct(QString);
    void slotUpdateUserModel(QString);
    void slotDiscardSocket();                               //
    void slotDisplayError(QAbstractSocket::SocketError);    //

private:
    QTcpSocket *mSocket = nullptr;                          //
    Auth mAuth;                                             //
    int mUserCount = 0;                                     //
    int mUserIndex = 0;                                     //
    QStandardItemModel* mUsersModel = nullptr;              // модель для таблицы пользователей
};

#endif // TCPHANDLER_H
