#include "handler.h"
#include <QFile>

// конструктор
Handler::Handler() {
}

// деструктор
Handler::~Handler() {
    if (mTCPhandler != nullptr)
        delete mTCPhandler;
    mAccessHandler.stop();
    mAccessHandler.quit();
    mAccessHandler.wait();
}

// включение обработчика
void Handler::start(ldapcore::QLdap *ldap) {

    // запуск обработчика доступа
    mLDAP = ldap;
    mAccessHandler.work(mLDAP);

    // запуск обработчика TCP
    mTCPhandler = new TCPhandler(mLDAP->getClientPort());
    connect(mTCPhandler, SIGNAL(signalAuth(QTcpSocket*, QString)),
                        this, SLOT(slotAuth(QTcpSocket*, QString)));
    connect(mTCPhandler, SIGNAL(signalUpdStruct(QTcpSocket*, QString)),
                        this, SLOT(slotUpdStruct(QTcpSocket*, QString)));
    connect(mTCPhandler, SIGNAL(signalUpdUsers(QTcpSocket*)),
                        this, SLOT(slotUpdUsers(QTcpSocket*)));
    connect(mTCPhandler, SIGNAL(signalEditAccess(QTcpSocket*, QString)),
                        this, SLOT(slotEditAccess(QTcpSocket*, QString)));
    connect(mTCPhandler, SIGNAL(signalDelAccess(QTcpSocket*, QString)),
                        this, SLOT(slotDelAccess(QTcpSocket*, QString)));
    connect(mTCPhandler, SIGNAL(signalResetPwd(QTcpSocket*, QString)),
                        this, SLOT(slotResetPwd(QTcpSocket*, QString)));
    connect(mTCPhandler, SIGNAL(signalConnectError()),
                        this, SIGNAL(signalConnectError()));

    // проверка готовности обработчика TCP
    if (!mTCPhandler->getReady())
        slotSendError();
}

// отключение обработчика
void Handler::stop() {
    mAccessHandler.stop();
    mAccessHandler.quit();
    mAccessHandler.wait();
    if (mTCPhandler != nullptr)
        delete mTCPhandler;
}

// поиск пользователя по указанному условию
int Handler::searchUsers(const QString str) {

    // подготовка результатов поиска
    mUserList.clear();
    mLDAP->setSearchResult(&mUserList);

    // поиск всех
    if (str.isEmpty())
        return mLDAP->userSearch("");

    // поиск по имени
    else
        return mLDAP->userSearchByName(str + "*");
}

// отправка ответа - результаты аторизации
void Handler::slotAuth(QTcpSocket *socket, const QString &str) {

    // успешная авторизация
    if (str.split(" ").at(0) == mLDAP->getLogin()
        && str.split(" ").at(1) == mLDAP->getPassword()) {
        mTCPhandler->responseAuth(socket, "success");
        qInfo() << QString("Client with sockd:%1 has been successfully authorized")
                       .arg(socket->socketDescriptor());
    }

    // неудачная авторизация
    else {
        mTCPhandler->responseAuth(socket, "error");
        qInfo() << QString("Client with sockd:%1 is not authorized")
                       .arg(socket->socketDescriptor());
    }
}

// отправка ответа - структура таблицы
void Handler::slotUpdStruct(QTcpSocket *socket, const QString &str) {

    // поиск по контейнеру пользователей
    int result = searchUsers(str);

    // поиск пользователей
    if (result != LDAP_SUCCESS) {

        // попытка переподключения
        mLDAP->close();
        if (mLDAP->connect() == LDAP_SUCCESS) {
            result = searchUsers(str);

            // создание структуры таблицы
            if (result == LDAP_SUCCESS)
                slotStructToStr(socket);
            else slotSendError();
        }
        else
            slotSendError();
    }
    else
        slotStructToStr(socket);
}

// преобразование структуры таблицы в строку
void Handler::slotStructToStr(QTcpSocket *socket) {
    QString str = "";

    // число пользователей для таблицы
    str += QString::number(mUserList.size());

    // формирование строки доступов с разделителями
    for (int j = 0; j < 15; j++) {
        str += "|";
        str += mLDAP->getAttributes(j + 1).first;
    }

    // обнуление счетчика отправленных пользователей
    mUserIndex = 0;

    // отправка структуры
    mTCPhandler->responseUpdStruct(socket, str);
}

// отправка ответа - информация об очередном пользователе
void Handler::slotUpdUsers(QTcpSocket *socket) {
    int i = 0;
    QString str = "";

    // цикл по всем пользователям
    for (auto e = mUserList.begin(); e != mUserList.end(); ++e) {

        // выбор очередного пользователя по счетчику отправленных
        if (i == mUserIndex) {
            ldapcore::QLdapEntry a = mUserList.at(i);
            ldapcore::QLdapUser *user = new ldapcore::QLdapUser(&a);

            // формирование строки с параметрами
            str += user->getDisplayName();
            for (int j = 0; j < 15; j++) {
                str +="|";
                if (mLDAP->getAttributes(j + 1).first != "empty")
                    str += user->getAttribute(j + 1,
                                              mLDAP->getAttributes(j + 1).second);
            }

            // отправка строки клиенту, перевод счетчика пользователей
            mUserIndex++;
            mTCPhandler->responseUpdUsers(socket, str);
            delete user;
            break;
        }
        i++;
    }
}

// редактирование доступа для пользователя
void Handler::slotEditAccess(QTcpSocket *socket, const QString &str)
{
    // поиск пользователя
    int result = searchUsers(str.split("|").at(2));

    // если первая попытка с ошибкой - переподключение
    if (result != LDAP_SUCCESS) {

        // попытка переподключения
        mLDAP->close();

        // успешное переподключение - редактирование доступа
        if (mLDAP->connect() == LDAP_SUCCESS) {
            result = searchUsers(str.split("|").at(2));

            // успешный поиск очередного пользователя
            if (result == LDAP_SUCCESS)
                slotEditAccessSingle(socket, str);

            // ошибка поиска
            else
                slotSendError();
        }

        // ошибка соединения
        else
            slotSendError();
    }

    // успешный поиск - редактирование доступа
    else
        slotEditAccessSingle(socket, str);
}

// отправка ответа - установка доступа
void Handler::slotEditAccessSingle(QTcpSocket *socket, const QString &str)
{

    // контрольная проверка, что пользователь нашелся
    if (!mUserList.empty()) {

        // определение аттрибута и его значения
        ldapcore::QLdapEntry user = mUserList.at(0);
        ldapcore::QLdapUser u = ldapcore::QLdapUser(&user);
        int index = str.split("|").at(0).toInt();
        QString value = str.split("|").at(1);

        // установка доступа
        mAccessHandler.addAttrGroup(&u, index, value);
    }

    // отправка ответа по результату
    mTCPhandler->responseEditAccess(socket, str);
}

// отключение доступа у пользователя
void Handler::slotDelAccess(QTcpSocket *socket, const QString &str)
{
    // поиск пользователя
    int result = searchUsers(str.split("|").at(1));

    // если первая попытка с ошибкой - переподключение
    if (result != LDAP_SUCCESS) {

        // попытка переподключения
        mLDAP->close();
        if (mLDAP->connect() == LDAP_SUCCESS) {
            result = searchUsers(str.split("|").at(1));

            // успешный поиск - отключение доступа
            if (result == LDAP_SUCCESS)
                slotDelAccessSingle(socket, str);

            // ошибка при поиске
            else
                slotSendError();
        }

        // ошибка соединения
        else
            slotSendError();
    }

    // успешное подключение - закрытие доступа
    else
        slotDelAccessSingle(socket, str);
}

// отправка ответа - отключение доступа
void Handler::slotDelAccessSingle(QTcpSocket *socket, const QString &str)
{
    // контрольная проверка, что пользователь нашелся
    if (!mUserList.empty()) {

        // определение обнуляемого атрибута доступа
        ldapcore::QLdapEntry user = mUserList.at(0);
        ldapcore::QLdapUser u = ldapcore::QLdapUser(&user);
        int index = str.split("|").at(0).toInt();

        // отключение доступа
        mAccessHandler.deleteAttrGroup(&u, index);
    }
    mTCPhandler->responseDelAccess(socket, str);
}

// сброс пароля с установкой пароля по умолчанию
void Handler::slotResetPwd(QTcpSocket *socket, const QString &str) {

    // поиск пользователя
    int result = searchUsers(str);
    if (result != LDAP_SUCCESS) {

        // попытка переподключения
        mLDAP->close();

        // успешное переподключение
        if (mLDAP->connect() == LDAP_SUCCESS) {
            result = searchUsers(str);

            // успешный поиск - сброс пароля
            if (result == LDAP_SUCCESS)
                slotResetPwdSingle(socket);

            // ошибка при поиске
            else
                slotSendError();
        }

        // ошибка соединения
        else
            slotSendError();
    }

    // успешный поиск - сброс пароля
    else
        slotResetPwdSingle(socket);

}

// отправка ответа - сброс пароля
void Handler::slotResetPwdSingle(QTcpSocket *socket) {

    // контрольная проверка, что пользователь нашелся
    if (!mUserList.empty()) {

        // сброс пароля по имени пользователя
        ldapcore::QLdapEntry user = mUserList.at(0);
        ldapcore::QLdapUser u = ldapcore::QLdapUser(&user);
        mLDAP->resetPassword(u.getSamAccountName());
        mTCPhandler->responseResetPwd(socket, u.getSamAccountName());
    }
    else
        mTCPhandler->responseResetPwd(socket, "not found");
}

// отправка сообщения об ошибке соединения
void Handler::slotSendError()
{
    emit signalConnectError();
}
