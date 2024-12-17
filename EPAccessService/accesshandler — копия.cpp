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
    //delete mTCPhandler;
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
    //connect
    //connect
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
    if (mTCPhandler != nullptr)                                 // уничтожение обработчика ТСР
        delete mTCPhandler;
}

//
void AccessHandler::slotAuth(QTcpSocket *socket, QString str) {

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

void AccessHandler::slotUpdStruct(QTcpSocket *socket, QString str) {

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

//
int AccessHandler::searchUsers(QString str) {
    mUserList.clear();
    mLDAP->setSearchResult(&mUserList);

    // поиск всех
    if (str.isEmpty())
        return mLDAP->userSearch("");

    // поиск по имени
    else
        return mLDAP->userSearchByName(str + "*");
}

// преобразование
void AccessHandler::slotStructToStr(QTcpSocket *socket) {
    QString str = "";
    str += QString::number(mUserList.size());
    for (int j = 0; j < 15; j++) {
        str += "|";
        str += mLDAP->getAttributes(j + 1).first;
    }
    mUserIndex = 0;
    mTCPhandler->responseUpdStruct(socket, str);
}

//
void AccessHandler::slotUpdUsers(QTcpSocket *socket) {

    //цикл по всем пользователям
    int i = 0;
    QString str = "";
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

            // отправка строки клиенту
            mUserIndex++;
            mTCPhandler->responseUpdUsers(socket, str);
            delete user;
            break;
        }
        i++;
    }
}

//

//

//

//


//
void AccessHandler::slotResetPwd(QTcpSocket *socket, QString str) {

    // поиск пользователей
    int result = searchUsers(str);
    if (result != LDAP_SUCCESS) {

        // попытка переподключения
        mLDAP->close();
        if (mLDAP->connect() == LDAP_SUCCESS) {
            result = searchUsers(str);

            //
            if (result == LDAP_SUCCESS)
                slotResetSingle(socket);
            else
                emit signalConnectError();
        }
        else
            emit signalConnectError();
    }
    else
        slotResetSingle(socket);

}

//
void AccessHandler::slotResetSingle(QTcpSocket *socket) {

    // контрольная проверка, что пользователь нашелся
    if (!mUserList.empty()) {

        ldapcore::QLdapEntry user = mUserList.at(0);
        ldapcore::QLdapUser u = ldapcore::QLdapUser(&user);

        // процесс powershell
        QProcess p;
        QString path = "C:/Windows/system32/WindowsPowerShell/v1.0/powershell.exe";
        QStringList commands;
        QByteArray out;

        // разблокировка пользователя
        commands.clear();
        commands.append("Unlock-ADAccount -Identity '" +
                            u.getSamAccountName() + "'");
        p.start(path, commands);
        p.waitForFinished();
        out = p.readAllStandardOutput().trimmed();

        // успешная разблокировка пользователя - ограничение срока действия пароля
        if (QString(out).isEmpty()) {

            // ограничение срока действия пароля
            qDebug() << "User" << u.getSamAccountName() << "is unlocked";
            commands.clear();
            commands.append("Set-ADUser -Identity '"
                            + u.getSamAccountName()
                            + "' -PasswordNeverExpires:$false");
            p.start(path, commands);
            p.waitForFinished();
            out = p.readAllStandardOutput().trimmed();

            // успешное ограничение срока действия пароля - разрешение на смену пароля
            if (QString(out).isEmpty()) {

                // разрешена смена пароля
                qDebug() << "Editing password's period for user" << u.getSamAccountName();
                commands.clear();
                commands.append("Set-ADUser -Identity '"
                                + u.getSamAccountName()
                                + "' -CannotChangePassword:$false");
                p.start(path, commands);
                p.waitForFinished();
                out = p.readAllStandardOutput().trimmed();

                // успешное разрешение на смену пароля - сброс пароля
                if (QString(out).isEmpty()) {

                    // сброс пароля
                    qDebug() << "Access editing password for user" << u.getSamAccountName();
                    commands.clear();
                    commands.append("Set-ADAccountPassword '" + u.getSamAccountName() +
                                    "' -Reset -NewPassword "
                                    "(ConvertTo-SecureString -AsPlainText 'Qq123123' "
                                    "-Force -Verbose)");
                    p.start(path, commands);
                    p.waitForFinished();
                    out = p.readAllStandardOutput().trimmed();

                    // успешный сброс пароля - требование смены пароля
                    if (QString(out).isEmpty()) {

                        // требование смены пароля
                        qDebug() << "Reset password for user" << u.getSamAccountName();
                        commands.clear();
                        commands.append("Set-ADuser -Identity '"
                                        + u.getSamAccountName()
                                        + "' -ChangePasswordAtLogon:$true");

                        p.start(path, commands);
                        p.waitForFinished();
                        out = p.readAllStandardOutput().trimmed();
                    }

                    //
                    if (QString(out).isEmpty()) {
                        qDebug() << "Requirement to edit default password for user" << u.getSamAccountName();
                        qInfo() << "Complete reset password for user" << u.getSamAccountName();
                        mTCPhandler->responseResetPwd(socket, u.getDisplayName());
                    }
                    else {
                        qCritical() << "Error requirement to edit default password for user"
                                    << u.getSamAccountName();
                        emit signalConnectError();
                    }
                }
                else {
                    qCritical() << "Error access editing password for user" << u.getSamAccountName();
                    emit signalConnectError();
                }
            }
            else {
                qCritical() << "Error editing password's period for user" << u.getSamAccountName();
                emit signalConnectError();
            }
        }
        else {
            qCritical() << "Error unlock user" << u.getSamAccountName();
            emit signalConnectError();
        }
    }
    else
        mTCPhandler->responseResetPwd(socket, "not found");

}



// выполнение по таймеру проверки доступа
void AccessHandler::slotCheckAccess() {

    // доступ проверяется в опр. время и в случае, если первая пока подключения к LDAP была неудачная
    if (mLDAP->getCheckingTime() == QTime::currentTime().toString("hh:mm") || isRetryCheckingAccess) {

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



// обработка доступа с очисткой
int AccessHandler::clearAccess(ldapcore::QLdapEntryList *userSearchResult) {

    int i = 0;
    QString str = "";

    // инициализация текущей даты
    QDate currentDate = QDate::fromString(QDate::currentDate().toString("dd.MM.yy"), "dd.MM.yy");
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
                            if (!deleteAttrGroup(user, attrPair.first, str,
                                                 j + 1, attrPair.second))
                                return 1;
                        }
                    }

                    // удаление доступа в случае некорректной даты
                    else {
                        if (!deleteAttrGroup(user, attrPair.first, str,
                                                j + 1, attrPair.second))
                            return 1;
                    }
                }
            }
        }
        delete user;
        i++;
    }

    return 0;
}

// удаление аттрибута и членства в группах
bool AccessHandler::deleteAttrGroup(ldapcore::QLdapUser *user,
                                    const QString &attrName,
                                    const QString &attrValue,
                                    const int index, QStringList groups) {

    int  res = 0;

    // очистка аттрибута
    if (!attrValue.isEmpty()) {
        qInfo() << "Clear user"
                << user->getSamAccountName() << "from" << attrName
                << "(value is '" + attrValue + "')";
        res = mLDAP->clearAttribute(user->getSamAccountName(), index);
    }
    if (res == 0) {

        // удаление пользователя из всех групп
        bool success = true;
        for (int i = 0; i < groups.size(); i++) {
            if (user->getMemberOf().contains(groups.at(i))) {
                qInfo() << "Delete user" << user->getSamAccountName()
                << "from" << groups.at(i);
                res = mLDAP->deleteUserFromGroup(user->getSamAccountName(),
                                                 groups.at(i));
                if (res != 0) success = false;
            }
        }
        return success;
    }
    else return false;
}
