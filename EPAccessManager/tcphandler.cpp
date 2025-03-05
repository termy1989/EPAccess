#include "tcphandler.h"

// конструктор
TCPhandler::TCPhandler() {

    // инициализация модели таблицы
    mUsersModel = new QStandardItemModel(1, 1);
}

// деструктор
TCPhandler::~TCPhandler() {

    // закрытие сокетов
    if (mSocket->isOpen())
        mSocket->close();
    if (mSocket != NULL)
        delete mSocket;
    delete mUsersModel;
}

// подключение к серверу и авторизация
void TCPhandler::authOnServer(const Auth &auth) {

    // инициализация сокета
    slotInitSocket();

    // подключение к серверу
    QStringList authList = QString::fromStdString(auth.url).split(":");
    if (authList.size() == 2) {
        mSocket->connectToHost(authList.at(0),authList.at(1).toInt());

        // попытка соединения
        if (mSocket->waitForConnected())
            sendAuthReq(auth);
    }

    // ошибка авторизации
    else
        emit signalConnectError("Authorized failed!");
}

// отправка запроса на авторизацию
void TCPhandler::sendAuthReq(const Auth &auth) {
    slotSendRequest("auth", QString::fromStdString(auth.login)
                            + " "
                            + QString::fromStdString(auth.password));
}

// отправка запроса на список пользователей
void TCPhandler::sendUpdStructReq(const QString &str) {
    slotSendRequest("update_s", str);
}

// отправка запроса на очередного пользователя
void TCPhandler::sendUpdUsersReq() {
    slotSendRequest("update_u", "update_u");
}

// отправка запроса на установку доступа
void TCPhandler::sendEditAccessReq(const quint8 attribute,
                                   const QStringList &userlist) {

    // формирование списка пользователей и параметров доступа
    mUserList = userlist;
    QString attr = QString::number(attribute);
    QString value = userlist.at(0);
    mUserList.removeAt(0);

    // сброс счетчика пользователей
    mUserIndex = 0;

    // формирование и отправка строки
    QString req = attr + "|" + value + "|" + mUserList.at(mUserIndex);
    slotEditAccess(req);
}

// отправка запроса на отключение доступа
void TCPhandler::sendDelAccessReq(const quint8 attribute,
                                  const QStringList &userlist)
{

    // формирование списка пользователей
    mUserList = userlist;
    QString attr = QString::number(attribute);

    // сброс счетчика пользователей
    mUserIndex = 0;

    // формирование и отправка строки
    QString req = attr + "|" + mUserList.at(mUserIndex);
    slotDelAccess(req);
}

// отправка запроса на сброс пароля
void TCPhandler::sendResetReq(const QStringList &userlist) {
    mUserList = userlist;
    mUserIndex = 0;
    slotResetPassword();
}

// отправка запросов
void TCPhandler::slotSendRequest(const QString &head,
                                 const QString &data) {

    // сокет в наличии
    if(mSocket) {

        // сокет открыт
        if(mSocket->isOpen()) {

            // формирование потока данных
            QDataStream socketStream(mSocket);
            socketStream.setVersion(QDataStream::Qt_6_8);

            // формирование заголовка с типом запроса
            QByteArray header;
            header.prepend(QString("reqType:" + head + ",fileName:null,fileSize:%1;")
                               .arg(data.size()).toUtf8());
            header.resize(128);

            // отправка потока данных через сокет
            QByteArray byteArray = data.toUtf8();
            byteArray.prepend(header);
            socketStream << byteArray;
            //socket->flush();
            isWaitResponse = true;
        }
        else
            emit signalConnectError("Socket doesn't seem to be opened");
    }
    else
        emit signalConnectError("Not connected");
}

// прием ответа от сервера
void TCPhandler::slotReadSocket() {

    isWaitResponse = false;

    // данные
    QByteArray buffer;

    // формирование потока данных
    QDataStream socketStream(mSocket);
    socketStream.setVersion(QDataStream::Qt_6_8);

    // передача данных клиенту от сервера
    socketStream.startTransaction();
    socketStream >> buffer;
    if(!socketStream.commitTransaction()) return;

    // обработка принятых данных
    slotResponseHandler(buffer);
}

// обработка ответа на запрос
void TCPhandler::slotResponseHandler(const QByteArray &buff) {

    // парсинг заголовка
    QString header = buff.mid(0,128);
    QString respType = header.split(",")[0].split(":")[1];
    QByteArray buffer = buff.mid(128);
    QString data = QString::fromStdString(buffer.toStdString());

    // ответ на запрос авторизации
    if (respType == "auth")
        slotAuth(data);

    // ответ на запрос структуры таблицы
    else if (respType == "update_s")
        slotCreateUserStruct(data);

    // ответ на запрос списка пользователей
    else if (respType == "update_u")
        slotUpdateUserModel(data);

    // ответ на запрос изменения доступа
    else if (respType == "access_edit") {
        mUserIndex++;
        slotEditAccess(data);
    }

    // ответ на запрос отключения доступа
    else if (respType == "access_del") {
        mUserIndex++;
        slotDelAccess(data);
    }

    // ответ на запрос сброса пароля
    else if (respType == "reset_pwd") {
        mUserIndex++;
        slotResetPassword();
    }
}

// обработка авторизации
void TCPhandler::slotAuth(const QString &data)
{
    if (data == "success")
        emit signalAuthSuccess();
    else
        emit signalConnectError("Authorization failed!");
}

// создание структуры таблицы
void TCPhandler::slotCreateUserStruct(const QString &data) {

    // определение числа пользователей и обнуление счетчика пользователей
    mUserIndex = 0;
    mUserCount = data.split("|").at(0).toInt();

    // отправка аттрибутов
    emit signalUpdateAttributes(data.split("|"));

    // очистка модели таблицы пользователей
    mUsersModel->clear();
    mUsersModel->setColumnCount(16);

    // установка заголовков столбцов
    mUsersModel->setHeaderData(0, Qt::Horizontal, "Ф.И.О.");
    for (int i = 1; i < data.split("|").size(); i++)
        mUsersModel->setHeaderData(i, Qt::Horizontal,
                                   data.split("|").at(i));

    // отправка основному окну пустой структуры таблицы
    emit signalUpdateUserList(mUsersModel);

    // отправка запроса на пользователей, если они есть в списке
    if (mUserCount != 0) {
        mUsersModel->setRowCount(mUserCount);
        sendUpdUsersReq();
    }
}

// запрос на информацию о пользователе
void TCPhandler::slotUpdateUserModel(const QString &data) {

    // проверка, сколько пользователей принято
    if (mUserIndex <= mUserCount) {

        // установка в модель параметров очередного пользователя
        mUsersModel->setItem(mUserIndex, 0,
                             new QStandardItem(data.split("|").at(0)));
        for (int i = 1; i <= 15; i++) {
            mUsersModel->setItem(mUserIndex, i,
                                 new QStandardItem(data.split("|").at(i)));
            mUsersModel->setData(mUsersModel->index(mUserIndex, i),
                                 Qt::AlignCenter, Qt::TextAlignmentRole);
        }
        mUserIndex++;
        emit signalUpdateProgress(mUserIndex, mUserCount,
                                  data.split("|").at(0),
                                  "Получение списка пользователей");

        // отправка модели по готовности
        if (mUserIndex == mUserCount) {
            mUserIndex = 0;
            emit signalUpdateUserList(mUsersModel);
        }

        // запрос на очередного пользователя
        else sendUpdUsersReq();

    }
}

// запрос на редактирование доступа
void TCPhandler::slotEditAccess(const QString &data)  {

    // проверка, сколько пользователей получили доступ
    if (mUserIndex <= mUserList.size()) {

        // доступ получили все - завершение
        if (mUserIndex == mUserList.size()) {
            emit signalUpdateProgress(mUserIndex, mUserList.size(),
                                      mUserList.at(mUserIndex - 1),
                                      "Установка значения extensionAttribute"
                                      + data.split("|").at(0) + " в "
                                      + data.split("|").at(1));
            mUserIndex = 0;
            emit signalOperationReady(false);
        }

        // доступ получили не все - продолжение
        else {
            emit signalUpdateProgress(mUserIndex, mUserList.size(),
                                      mUserList.at(mUserIndex),
                                      "Установка значения extensionAttribute"
                                          + data.split("|").at(0) + " в "
                                          + data.split("|").at(1));
            QString str = "";
            str = data.split("|").at(0)
                  + "|" + data.split("|").at(1)
                  + "|" + mUserList.at(mUserIndex);
            slotSendRequest("access_edit", str);
        }
    }
}

// запрос на отключение доступа
void TCPhandler::slotDelAccess(const QString &data) {

    // проверка, сколько пользователей получили доступ
    if (mUserIndex <= mUserList.size()) {

        // доступ отключен у всех - завершение
        if (mUserIndex == mUserList.size()) {
            emit signalUpdateProgress(mUserIndex, mUserList.size(),
                                      mUserList.at(mUserIndex - 1),
                                      "Отключение доступа");
            mUserIndex = 0;
            emit signalOperationReady(false);
        }

        // доступ отключени не у всех - продолжение
        else {
            emit signalUpdateProgress(mUserIndex, mUserList.size(),
                                      mUserList.at(mUserIndex),
                                      "Отключение доступа");
            QString str = "";
            str = data.split("|").at(0)
                  + "|" + mUserList.at(mUserIndex);
            slotSendRequest("access_del", str);
        }
    }
}

// сброс пароля
void TCPhandler::slotResetPassword() {

    // проверка, сколько пользователей сбросили пароли
    if (mUserIndex <= mUserList.size()) {

        // пароль сброшен у всех - завершение
        if (mUserIndex == mUserList.size()) {
            emit signalUpdateProgress(mUserIndex, mUserList.size(),
                                      mUserList.at(mUserIndex - 1),
                                      "Сброс пароля");
            mUserIndex = 0;
            emit signalOperationReady(true);
        }

        // пароль сброшен не у всех - продолжение
        else {
            emit signalUpdateProgress(mUserIndex, mUserList.size(),
                                      mUserList.at(mUserIndex),
                                      "Сброс пароля");
            slotSendRequest("reset_pwd", mUserList.at(mUserIndex));
        }
    }
}

// разрыв соединения с сервером
void TCPhandler::slotDiscardSocket() {

    if (mSocket != NULL) {
        mSocket->deleteLater();
        mSocket=nullptr;
    }
}

// инициализация сокета
void TCPhandler::slotInitSocket() {
    mSocket = new QTcpSocket(this);
    connect(mSocket, SIGNAL(readyRead()), this, SLOT(slotReadSocket()));
    connect(mSocket, SIGNAL(disconnected()), this, SLOT(slotDiscardSocket()));
    connect(mSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)),
                    this, SLOT(slotDisplayError(QAbstractSocket::SocketError)));

    // настройка прокси для сокета
    if (mAuth.proxy.type != 0) {
        QNetworkProxy proxy;

        if (mAuth.proxy.type == 1)
            proxy.setType(QNetworkProxy::HttpProxy);
        else if (mAuth.proxy.type == 2)
            proxy.setType(QNetworkProxy::HttpCachingProxy);
        else if (mAuth.proxy.type == 3)
            proxy.setType(QNetworkProxy::Socks5Proxy);

        proxy.setHostName(QString::fromStdString(mAuth.proxy.url));
        proxy.setPort(mAuth.proxy.port);

        if (mAuth.proxy.login != "" && mAuth.proxy.password != "") {
            proxy.setUser(QString::fromStdString(mAuth.proxy.login));
            proxy.setPassword(QString::fromStdString(mAuth.proxy.password));
        }

        QNetworkProxy::setApplicationProxy(proxy);
        //mSocket->setProxy(proxy);
    }
}

// обработка ошибок соединения
void TCPhandler::slotDisplayError(QAbstractSocket::SocketError socketError) {

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        if (isWaitResponse)
            emit signalConnectError("Remote Host Closed Error!");
        break;
    case QAbstractSocket::HostNotFoundError:
        emit signalConnectError("The host was not found. Please check "
                                "the host name and port settings");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        emit signalConnectError("The connection was refused by the peer. "
                                "Make sure QTCPServer is running, and check "
                                "that the host name and port settings are correct");
        break;
    default:
        emit signalConnectError(QString("The following error occurred: %1.")
                                                .arg(mSocket->errorString()));
        break;
    }
}
