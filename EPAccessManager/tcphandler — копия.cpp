#include "tcphandler.h"

// конструктор
TCPhandler::TCPhandler(Auth auth) {

    // инициализация сокета
    mAuth = auth;
    mSocket = new QTcpSocket(this);
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(slotReadSocket()));
    connect(mSocket, SIGNAL(disconnected()), this, SLOT(slotDiscardSocket()));
    connect(mSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)),
                this, SLOT(slotDisplayError(QAbstractSocket::SocketError)));

    //
    mUsersModel = new QStandardItemModel(1, 1);
}

// деструктор
TCPhandler::~TCPhandler() {
    // закрытие сокета
    /*if(mSocket->isOpen())
        mSocket->close();
    delete mUsersModel;*/
}

// подключение к серверу и авторизация
void TCPhandler::authOnServer()
{

    //
    mSocket->connectToHost(QString::fromStdString(mAuth.url).split(":").at(0),
                           QString::fromStdString(mAuth.url).split(":").at(1).toInt());

    //
    if (mSocket->waitForConnected())
        sendAuthReq();

    //
    else
        emit signalConnectError(QString("The following error occurred: %1.")
                                                .arg(mSocket->errorString()));
}

// запрос на авторизацию
void TCPhandler::sendAuthReq()
{
    slotSendRequest("auth", QString::fromStdString(mAuth.login)
                            + " "
                            + QString::fromStdString(mAuth.password));
}

// отправка запроса на список пользователей
void TCPhandler::sendUpdStructReq(QString str)
{
    slotSendRequest("update_s", str);
}

//
void TCPhandler::sendUpdUsersReq() {
    slotSendRequest("update_u", "update_u");
}

//
void TCPhandler::sendResetReq(QString) {
    int a;
}

//
void TCPhandler::sendAccessReq(QString)
{
    int a;
}

//----------------------------------//
void TCPhandler::sendTestReq(QString str)
{
    slotSendRequest("test", str);
}

// отправка запросов
void TCPhandler::slotSendRequest(QString head, QString data) {

    if(mSocket)
    {
        if(mSocket->isOpen())
        {
            QDataStream socketStream(mSocket);
            socketStream.setVersion(QDataStream::Qt_6_8);

            QByteArray header;
            header.prepend(QString("reqType:" + head + ",fileName:null,fileSize:%1;")
                               .arg(data.size()).toUtf8());
            header.resize(128);

            QByteArray byteArray = data.toUtf8();
            byteArray.prepend(header);
            socketStream << byteArray;
            //socket->flush();
        }
        else
            qDebug() << "Socket doesn't seem to be opened";
    }
    else
        qDebug() << "Not connected";
}

void TCPhandler::slotReadSocket() {

    //
    QByteArray buffer;

    //
    QDataStream socketStream(mSocket);
    socketStream.setVersion(QDataStream::Qt_6_8);

    //
    socketStream.startTransaction();
    socketStream >> buffer;

    //
    if(!socketStream.commitTransaction())
    {
        QString message = QString("%1 :: Waiting for more data to come..")
                                            .arg(mSocket->socketDescriptor());
        qDebug() << message;
        //emit newMessage(message);
        return;
    }

    //
    slotResponseHandler(buffer);
}

//
void TCPhandler::slotResponseHandler(QByteArray buffer) {

    //
    QString header = buffer.mid(0,128);
    QString respType = header.split(",")[0].split(":")[1];
    buffer = buffer.mid(128);
    QString data = QString::fromStdString(buffer.toStdString());

    // ответ на запрос авторизации
    if (respType == "auth") {
        if (data == "success")
            sendUpdStructReq("");
        else
            emit signalConnectError("Authorization failed!");
    }

    // ответ на запрос структуры
    else if (respType == "update_s") {
        slotCreateUserStruct(data);
        emit signalUpdateUserStruct(data);
    }

    // ответ на запрос списка пользователей
    else if (respType == "update_u")
        slotUpdateUserModel(data);


    else {
        QString message = QString("%1 :: %2").arg(mSocket->socketDescriptor()).arg(QString::fromStdString(data.toStdString()));
        //emit newMessage(message);
        qDebug() << message;
    }
}

//
void TCPhandler::slotCreateUserStruct(QString data) {

    //
    mUserIndex = 0;
    mUserCount = data.split("|").at(0).toInt();
    qDebug() << mUserCount;

    // очистка модели таблицы пользователей
    mUsersModel->clear();

    //
    mUsersModel->setColumnCount(16);
    mUsersModel->setHeaderData(0, Qt::Horizontal, "Ф.И.О.");
    for (int i = 1; i < data.split("|").size(); i++) {
        mUsersModel->setHeaderData(i, Qt::Horizontal,
                                   data.split("|").at(i));
    }
    mUsersModel->setRowCount(mUserCount);

    //
    sendUpdUsersReq();

}

//
void TCPhandler::slotUpdateUserModel(QString data) {

    //qDebug() << data;
    if (mUserIndex <= mUserCount) {

        mUsersModel->setItem(mUserIndex, 0,
                             new QStandardItem(data.split("|").at(0)));
        for (int i = 1; i <= 15; i++) {
            mUsersModel->setItem(mUserIndex, i,
                                 new QStandardItem(data.split("|").at(i)));
            mUsersModel->setData(mUsersModel->index(mUserIndex, i),
                                 Qt::AlignCenter, Qt::TextAlignmentRole);
        }
        mUserIndex++;
        sendUpdUsersReq();

        if (mUserIndex == mUserCount) {
            signalUpdateUserList(mUsersModel);
            mUserIndex = 0;
        }
    }
}

//
void TCPhandler::slotDiscardSocket() {
    mSocket->deleteLater();
    mSocket=nullptr;
    //ui->statusBar->showMessage("Disconnected!");
}

//
void TCPhandler::slotDisplayError(QAbstractSocket::SocketError socketError) {

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        emit signalConnectError("Remote Host Closed Error!");
        break;
    case QAbstractSocket::HostNotFoundError:
        /*qDebug() << "The host was not found. Please check "
                    "the host name and port settings";*/
        emit signalConnectError("The host was not found. Please check "
                                "the host name and port settings");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        /*qDebug() << "The connection was refused by the peer. "
                    "Make sure QTCPServer is running, and check "
                    "that the host name and port settings are correct";*/
        emit signalConnectError("The connection was refused by the peer. "
                                "Make sure QTCPServer is running, and check "
                                "that the host name and port settings are correct");
        break;
    default:
        /*qDebug() << QString("The following error occurred: %1.")
                        .arg(mSocket->errorString());*/
        emit signalConnectError(QString("The following error occurred: %1.")
                                                .arg(mSocket->errorString()));
        break;
    }
}
