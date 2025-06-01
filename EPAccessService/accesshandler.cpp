#include "accesshandler.h"

// запуск потока
void AccessHandler::run() {
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), SLOT(slotCheckAccess()));
    connect(this, SIGNAL(stopTimer()), &timer, SLOT(stop()));
    timer.start(60000);
    exec();
}

// запуск обработчика
void AccessHandler::work(ldapcore::QLdap *ldap) {
    mLDAP = ldap;
    isRetryCheckingAccess = false;
    start();
}

// остановка таймера
void AccessHandler::stop() {
    emit stopTimer();
}

// проверка доступа
void AccessHandler::slotCheckAccess() {

    // доступ проверяется в опр. время и в случае, если первая пока подключения к LDAP была неудачная
    if (mLDAP->getCheckingTimeNight() == QTime::fromString(QTime::currentTime().toString("hh:mm"))
        || mLDAP->getCheckingTimeDay() == QTime::fromString(QTime::currentTime().toString("hh:mm"))
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

