#include "tcphandler.h"

// конструктор
TCPhandler::TCPhandler(int port) {

    // инициализация сервера
    mServer = new QTcpServer();

    // включение сервера
    if(mServer->listen(QHostAddress::Any, port)) {
        connect(mServer, SIGNAL(newConnection()),
                    this, SLOT(slotNewConnection()));
        isReady = true;
        qInfo() << "Listening on port "
                       + QString::number(port) + "...";
    }

    // неудачное включение
    else {
        qCritical() << "TCP:" << QString("Unable to start the server: %1.").arg(mServer->errorString());
        isReady = false;
    }
}

// деструктор
TCPhandler::~TCPhandler() {

    // отключение сокетов и сервера
    foreach (QTcpSocket* socket, mConnections) {
        socket->close();
        socket->deleteLater();
    }
    mServer->close();
    mServer->deleteLater();
}

// статус готовности обработчика
bool TCPhandler::getReady() {
    return isReady;
}

// отправка ответа на запрос авторизации
void TCPhandler::responseAuth(QTcpSocket *socket,
                              const QString &str) {
    slotSendResponse(socket, "auth", str);
}

// отправка структуры таблицы
void TCPhandler::responseUpdStruct(QTcpSocket *socket,
                                   const QString &str) {
    slotSendResponse(socket, "update_s", str);
}

// отправка списка пользователей
void TCPhandler::responseUpdUsers(QTcpSocket *socket,
                                  const QString &str) {
    slotSendResponse(socket, "update_u", str);
}

// отправка ответа на запрос настройки доступа
void TCPhandler::responseEditAccess(QTcpSocket *socket,
                                    const QString &str) {
    slotSendResponse(socket, "access_edit", str);
}

// отправка ответа на запрос отключения доступа
void TCPhandler::responseDelAccess(QTcpSocket *socket,
                                   const QString &str) {
    slotSendResponse(socket, "access_del", str);
}

// отправка пользователя со сброшенным паролем
void TCPhandler::responseResetPwd(QTcpSocket *socket,
                                  const QString &str) {
    slotSendResponse(socket, "reset_pwd", str);
}

// подключение нового клиента
void TCPhandler::slotNewConnection() {
    while (mServer->hasPendingConnections())
        slotAppendToSocketList(mServer->nextPendingConnection());
}

// добавление нового клиента в список активных подключений
void TCPhandler::slotAppendToSocketList(QTcpSocket *socket) {
    mConnections.insert(socket);
    connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadSocket()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(slotDiscardSocket()));
    connect(socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)),
                        this, SLOT(slotDisplayError(QAbstractSocket::SocketError)));
    qInfo() << QString("Client with sockd:%1 has just connected")
                   .arg(socket->socketDescriptor());
}

// чтение запроса от клиента
void TCPhandler::slotReadSocket() {

    // определение сокета-отправителя
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    // инициализация потока данных из сокета
    QByteArray buffer;
    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_6_8);

    // чтение потока данных
    socketStream.startTransaction();
    socketStream >> buffer;
    if(!socketStream.commitTransaction()) {
        QString message = QString("%1 - Waiting for more data to come...")
                                            .arg(socket->socketDescriptor());
        qDebug() << message;
        return;
    }

    // обработка полученных данных
    slotRequestHandler(socket, buffer);
}

// обработчик запросов от клиента
void TCPhandler::slotRequestHandler(QTcpSocket* socket, QByteArray buffer) {

    // чтение заголовка, определение типа запроса
    QString header = buffer.mid(0,128);
    QString reqType = header.split(",")[0].split(":")[1];
    buffer = buffer.mid(128);
    QString str = QString::fromStdString(buffer.toStdString());

    // запрос авторизации
    if(reqType == "auth") emit signalAuth(socket, str);

    // запрос структуры
    else if (reqType == "update_s") emit signalUpdStruct(socket, str);

    // запрос списка пользователей
    else if (reqType == "update_u") emit signalUpdUsers(socket);

    // запрос на изменение доступа
    else if (reqType == "access_edit") emit signalEditAccess(socket, str);

    // запрос на отключение доступа
    else if (reqType == "access_del") emit signalDelAccess(socket, str);

    // запрос на сброс пароля
    else if (reqType == "reset_pwd") emit signalResetPwd(socket, str);
}

// отправка ответа на запрос
void TCPhandler::slotSendResponse(QTcpSocket *socket,
                                  const QString &head,
                                  const QString &str) {

    // сокет существует
    if(socket) {

        // сокет открыт
        if(socket->isOpen()) {
            // инициализация потока данных для
            QDataStream socketStream(socket);
            socketStream.setVersion(QDataStream::Qt_6_8);

            // формирование заголовка
            QByteArray header;
            header.prepend(QString("respType:" + head + ",fileName:null,fileSize:%1;")
                               .arg(str.size()).toUtf8());
            header.resize(128);

            // кодировка utf для данных
            QByteArray byteArray = str.toUtf8();
            byteArray.prepend(header);

            // отправка данных в поток
            socketStream.setVersion(QDataStream::Qt_6_8);
            socketStream << byteArray;
        }
        else
            qCritical() << "TCP:" << "Socket doesn't seem to be opened";
    }
    else
        qCritical() << "TCP:" << "Not connected";
}

// отключение клиента
void TCPhandler::slotDiscardSocket() {

    // определение отключенного клиента
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());
    QSet<QTcpSocket*>::iterator it = mConnections.find(socket);

    // удаление клиента из списка
    if (it != mConnections.end()) {
        qInfo() << QString("A client has just disconnected").arg(socket->socketDescriptor());
        mConnections.remove(*it);
    }

    // удаление сокета
    socket->deleteLater();
}

// ошибки сети
void TCPhandler::slotDisplayError(QAbstractSocket::SocketError socketError) {
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qCritical() << "TCP:" << "The host was not found. Please check the host name and port settings";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qCritical() << "TCP:" << "The connection was refused by the peer. "
                                 "Make sure QTCPServer is running, and check "
                                 "that the host name and port settings are correct";
        break;
    default:
        QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
        qCritical() << "TCP:" << QString("The following error occurred: %1.").arg(socket->errorString());
        break;
    }
}
