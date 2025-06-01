#ifndef TCPHANDLER_H
#define TCPHANDLER_H

#include <QObject>
#include <QSet>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include "ldapcore/qldap.h"
#include "ldapcore/qldapuser.h"

// класс обработки запросов от менеджера
class TCPhandler : public QObject
{
    Q_OBJECT

public:
    TCPhandler(int port);                                       // конструктор
    ~TCPhandler();                                              // деструктор
    bool getReady();                                            // статус - готовность

    void responseAuth(QTcpSocket*, QString);                    // отправка ответа на запрос авторизации
    void responseUpdStruct(QTcpSocket*, QString);               // отправка ответа на запрос структуры таблицы
    void responseUpdUsers(QTcpSocket*, QString);                // отправка ответа на запрос о пользователя
    void responseEditAccess(QTcpSocket*, QString);              //
    void responseDelAccess(QTcpSocket*, QString);               //
    void responseResetPwd(QTcpSocket*, QString);                // отправ

signals:
    void signalAuth(QTcpSocket*, QString);                      //
    void signalUpdStruct(QTcpSocket*, QString);                 //
    void signalUpdUsers(QTcpSocket*);                           //
    void signalEditAccess(QTcpSocket*, QString);                //
    void signalDelAccess(QTcpSocket*, QString);                 //
    void signalResetPwd(QTcpSocket*, QString);                  //
    void signalConnectError();                                  // сигнал об ошибке соединения

private slots:
    void slotNewConnection();                                   // подключение нового клиента
    void slotAppendToSocketList(QTcpSocket*);                   // добавление нового клиента в список активных подключений
    void slotReadSocket();                                      // чтение запроса от клиента
    void slotRequestHandler(QTcpSocket*, QByteArray);           // обработка запроса
    void slotSendResponse(QTcpSocket*, QString, QString);       // отправка ответа на запрос
    void slotDiscardSocket();                                   // разрыв соединения с клиентом
    void slotDisplayError(QAbstractSocket::SocketError);        // ошибка сети


private:
    QTcpServer *mServer = nullptr;                              // сервер TCP
    QSet<QTcpSocket*> mConnections;                             // список активных подключений
    bool isReady = false;                                       // флаг общей готовности обработчика
    bool isBusy = false;                                        // флаг готовности к приему запроса
};

#endif // TCPHANDLER_H
