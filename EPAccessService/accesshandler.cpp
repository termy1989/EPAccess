#include "accesshandler.h"
#include <QFile>

// конструктор
AccessHandler::AccessHandler() {

    // инициализация и подключение таймера
    mTimer = new QTimer(nullptr);
    connect(this, SIGNAL(signalTimerStart(int)), mTimer, SLOT(start(int)));
    connect(this, SIGNAL(signalTimerStop()), mTimer, SLOT(stop()));
    connect(mTimer, SIGNAL(timeout()), this, SLOT(slotCheckAccess()));
}

// деструктор
AccessHandler::~AccessHandler() {
    delete mTimer;
    delete mTCPhandler;
}

// включение обработчика
void AccessHandler::start(ldapcore::QLdap *ldap) {

    // запуск таймера
    mLDAP = ldap;
    emit signalTimerStart(60000);

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
        emit signalConnectError();
}

// отключение обработчика
void AccessHandler::stop() {
    emit signalTimerStop();
    if (mTCPhandler != nullptr)
        delete mTCPhandler;
}

// обработка доступа с очисткой
int AccessHandler::clearAccess(const ldapcore::QLdapEntryList *userSearchResult) {

    int i = 0;
    QString str = "";

    // инициализация текущей даты
    QDate currentDate = QDate::fromString(
        QDate::currentDate().toString("dd.MM.yy"),
        "dd.MM.yy"
        );
    QPair<QString, QStringList> attrPair;

    // цикл по всем пользователям
    for (auto e = userSearchResult->begin();
         e != userSearchResult->end(); ++e) {

        ldapcore::QLdapEntry a = userSearchResult->at(i);
        ldapcore::QLdapUser *user = new ldapcore::QLdapUser(&a);

        // цикл по всем 15 аттрибутам
        for (int j = 0; j < 15; j++) {

            attrPair = mLDAP->getAttributes(j + 1);

            // аттрибут задействован
            if (attrPair.first != "empty") {

                // значение аттрибута у пользователя
                str = user->getAttribute(j + 1);

                // аттрибут не vip
                if (str != "vip") {

                    // проверка на даты корректность
                    QDate date = QDate::fromString(str, "dd.MM.yy");
                    if (date.isValid()) {

                        // проверка даты на просроченность
                        if (date < currentDate) {
                            if (!deleteAttrGroup(user, j + 1))
                                return 1;
                        }

                        // проверка
                        else if (!addAttrGroup(user, j + 1, str))
                            return 1;
                    }

                    // удаление доступа в случае некорректной даты
                    else {
                        if (!deleteAttrGroup(user, j + 1))
                            return 1;
                    }
                }
                else if (!addAttrGroup(user, j + 1, str))
                    return 1;
            }
        }
        delete user;
        i++;
    }

    return 0;
}

// добавление атрибута и членства в группе
bool AccessHandler::addAttrGroup(const ldapcore::QLdapUser *user,
                                 const int index,
                                 const QString value) {

    int  res = 0;

    // проверка, установлен ли нужный атрибут ранее
    if (user->getAttribute(index) != value) {

        // нужный атрибут не установлен - очистка атрибута, если он не пустой
        if (!user->getAttribute(index).isEmpty()) {
            qInfo() << "Clear user"
                    << user->getSamAccountName() << "from"
                    << mLDAP->getAttributes(index).first
                    << "(value is '" + user->getAttribute(index) + "')";
            res = mLDAP->clearAttribute(user->getSamAccountName(), index);
        }

        // установка нужного атрибута
        if (res == 0)
            res = mLDAP->setAttributeToUser(user->getSamAccountName(), index, value);
        else return false;
    }

    // добавление пользователя в группы
    if (res == 0) {
        bool success = true;

        // список групп, относящихся к типу доступа
        QStringList groups = mLDAP->getAttributes(index).second;

        // цикл по списку групп
        for (int i = 0; i < groups.size(); i++) {

            // добавление пользователя в группу, если еще не добавлен
            if (!user->getMemberOf().contains(groups.at(i))) {
                res = mLDAP->addUserToGroup(user->getSamAccountName(), groups.at(i));
                if (res != 0) success = false;
            }
        }
        return success;
    }
    else return false;
}

// удаление аттрибута и членства в группах
bool AccessHandler::deleteAttrGroup(const ldapcore::QLdapUser *user,
                                    const int index) {

    int  res = 0;

    // очистка аттрибута, если он не пустой
    if (!user->getAttribute(index).isEmpty()) {
        qInfo() << "Clear user"
                << user->getSamAccountName() << "from"
                << mLDAP->getAttributes(index).first
                << "(value is '" + user->getAttribute(index) + "')";
        res = mLDAP->clearAttribute(user->getSamAccountName(), index);
    }

    // удаление пользователя из всех групп данного доступа
    if (res == 0) {
        bool success = true;

        // список групп, относящихся к типу доступа
        QStringList groups = mLDAP->getAttributes(index).second;

        // цикл по списку групп
        for (int i = 0; i < groups.size(); i++) {

            // удаление пользователя из группы, если уже не удален
            if (user->getMemberOf().contains(groups.at(i))) {
                res = mLDAP->deleteUserFromGroup(user->getSamAccountName(),
                                                 groups.at(i));
                if (res != 0) success = false;
            }
        }
        return success;
    }
    else return false;
}

// поиск пользователя по указанному условию
int AccessHandler::searchUsers(const QString str) {

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


// выполнение по таймеру проверки доступа
void AccessHandler::slotCheckAccess() {

    // доступ проверяется в опр. время и в случае, если первая пока подключения к LDAP была неудачная
    if (mLDAP->getCheckingTime() == QTime::currentTime().toString("hh:mm")
        || isRetryCheckingAccess) {

        // первая или вторая попытка
        if (isRetryCheckingAccess)
            qInfo() << "Retrying checking access...";
        else
            qInfo() << "Start checking access...";

        // поиск по контейнерам пользователей
        ldapcore::QLdapEntryList *userSearchResult = new ldapcore::QLdapEntryList();
        mLDAP->setSearchResult(userSearchResult);
        int result = mLDAP->userSearch("");

        // ошибка при поиске запией - требуется переподключение
        if (result != LDAP_SUCCESS) {
            isRetryCheckingAccess = true;
            delete userSearchResult;
            emit signalConnectError();
        }

        // успешный поиск записей - обработка доступа
        else {
            isRetryCheckingAccess = false;
            int res = clearAccess(userSearchResult);
            if (res != 0) {
                delete userSearchResult;
                emit signalConnectError();
            }
            else {
                qInfo() << "End checking access";
                delete userSearchResult;
            }
        }
    }
}

// отправка ответа - результаты аторизации
void AccessHandler::slotAuth(QTcpSocket *socket, const QString &str) {

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
void AccessHandler::slotUpdStruct(QTcpSocket *socket, const QString &str) {

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
            else emit signalConnectError();
        }
        else
            emit signalConnectError();
    }
    else
        slotStructToStr(socket);
}

// преобразование структуры таблицы в строку
void AccessHandler::slotStructToStr(QTcpSocket *socket) {
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
void AccessHandler::slotUpdUsers(QTcpSocket *socket) {
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
void AccessHandler::slotEditAccess(QTcpSocket *socket, const QString &str)
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
                emit signalConnectError();
        }

        // ошибка соединения
        else
            emit signalConnectError();
    }

    // успешный поиск - редактирование доступа
    else
        slotEditAccessSingle(socket, str);
}

// отправка ответа - установка доступа
void AccessHandler::slotEditAccessSingle(QTcpSocket *socket, const QString &str)
{

    // контрольная проверка, что пользователь нашелся
    if (!mUserList.empty()) {

        // определение аттрибута и его значения
        ldapcore::QLdapEntry user = mUserList.at(0);
        ldapcore::QLdapUser u = ldapcore::QLdapUser(&user);
        int index = str.split("|").at(0).toInt();
        QString value = str.split("|").at(1);

        // установка доступа
        addAttrGroup(&u, index, value);
    }

    // отправка ответа по результату
    mTCPhandler->responseEditAccess(socket, str);
}

// отключение доступа у пользователя
void AccessHandler::slotDelAccess(QTcpSocket *socket, const QString &str)
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
                emit signalConnectError();
        }

        // ошибка соединения
        else
            emit signalConnectError();
    }

    // успешное подключение - закрытие доступа
    else
        slotDelAccessSingle(socket, str);
}

// отправка ответа - отключение доступа
void AccessHandler::slotDelAccessSingle(QTcpSocket *socket, const QString &str)
{
    // контрольная проверка, что пользователь нашелся
    if (!mUserList.empty()) {

        // определение обнуляемого атрибута доступа
        ldapcore::QLdapEntry user = mUserList.at(0);
        ldapcore::QLdapUser u = ldapcore::QLdapUser(&user);
        int index = str.split("|").at(0).toInt();

        // отключение доступа
        deleteAttrGroup(&u, index);
    }
    mTCPhandler->responseDelAccess(socket, str);
}

// сброс пароля с установкой пароля по умолчанию
void AccessHandler::slotResetPwd(QTcpSocket *socket, const QString &str) {

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
                emit signalConnectError();
        }

        // ошибка соединения
        else
            emit signalConnectError();
    }

    // успешный поиск - сброс пароля
    else
        slotResetPwdSingle(socket);

}

// отправка ответа - сброс пароля
void AccessHandler::slotResetPwdSingle(QTcpSocket *socket) {

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
