#ifndef TCPHANDLER_H
#define TCPHANDLER_H

#include <QObject>
#include <QSet>
#include <QTcpServer>
#include <QTcpSocket>
#include "ldapcore/qldap.h"
#include "ldapcore/qldapuser.h"

// класс обработки запросов от менеджера
class TCPhandler : public QObject
{
    Q_OBJECT

public:
    TCPhandler(int port);                                       // конструктор
    ~TCPhandler();                                              // деструктор
    bool getReady();                                            // статус готовности

    void responseAuth(QTcpSocket*, const QString&);             // отправка ответа на запрос авторизации
    void responseUpdStruct(QTcpSocket*, const QString&);        // отправка ответа на запрос структуры таблицы
    void responseUpdUsers(QTcpSocket*, const QString&);         // отправка ответа на запрос о пользователя
    void responseEditAccess(QTcpSocket*, const QString&);       // отправка ответа на запрос настройки доступа
    void responseDelAccess(QTcpSocket*, const QString&);        // отправка ответа на запрос отключения доступа
    void responseResetPwd(QTcpSocket*, const QString&);         // отправка ответа на запрос сброса пароля

signals:
    void signalAuth(QTcpSocket*, QString);                      // сигнал о получении запроса на авторизацию
    void signalUpdStruct(QTcpSocket*, QString);                 // сигнал о получении запроса на структуру таблицы
    void signalUpdUsers(QTcpSocket*);                           // сигнал о получении запроса на информацию о пользователе
    void signalEditAccess(QTcpSocket*, QString);                // сигнал о получении запроса на изменение доступа
    void signalDelAccess(QTcpSocket*, QString);                 // сигнал о получении запроса на отключение доступа
    void signalResetPwd(QTcpSocket*, QString);                  // сигнал о получении запроса на сброс пароля
    void signalConnectError();                                  // сигнал об ошибке соединения

private slots:
    void slotNewConnection();                                   // подключение нового клиента
    void slotAppendToSocketList(QTcpSocket*);                   // добавление нового клиента в список активных подключений
    void slotReadSocket();                                      // чтение запроса от клиента
    void slotRequestHandler(QTcpSocket*, const QByteArray&);    // обработка запроса
    void slotSendResponse(QTcpSocket*, const QString&,
                                        const QString&);        // отправка ответа на запрос
    void slotDiscardSocket();                                   // разрыв соединения с клиентом
    void slotDisplayError(QAbstractSocket::SocketError);        // ошибка сети


private:
    QTcpServer *mServer = nullptr;                              // сервер TCP
    QSet<QTcpSocket*> mConnections;                             // список активных подключений
    bool isReady = false;                                       // флаг общей готовности обработчика
    bool isBusy = false;                                        // флаг готовности к приему запроса
};

#endif // TCPHANDLER_H
