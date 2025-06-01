#include "qldap.h"

namespace ldapcore {

// конструктор
QLdap::QLdap(QString filename) {
    mFilename = filename;
}

// деструктор
QLdap::~QLdap() {
    mLDAP = NULL;
}

// инициализация соединения
int QLdap::init() {

    int result;
    mLDAP = NULL;

    // создание соединения по url
    std::string str = ("ldap://" + mUrl + ":389").toStdString();
    const char *aurl = str.c_str();
    result = ldap_create(&mLDAP);

    // ошибка при установке соединения
    if (result != LDAP_SUCCESS) {
        qCritical() << "ldap_create() error:" << ldap_err2string(result);
        return result;
    }

    // установка базовых свойств соединения
    if (!mUrl.isEmpty()) {
        result = ldap_set_option(mLDAP, LDAP_OPT_URI, aurl);
        if (result != LDAP_SUCCESS) {
            qCritical() << "ldap_set_option() url error:" << ldap_err2string(result);
            return result;
        }
    }

    const int version = LDAP_VERSION3;
    result = ldap_set_option(mLDAP, LDAP_OPT_PROTOCOL_VERSION, &version);
    if (result != LDAP_SUCCESS)
        qCritical() << "ldap_set_option() ver LDAP_OPT_PROTOCOL_VERSION error:" << ldap_err2string(result);

    const int reff = 0;
    ldap_set_option(mLDAP, LDAP_OPT_REFERRALS, &reff);
    if (result != LDAP_SUCCESS)
        qCritical() << "ldap_set_option() ref LDAP_OPT_REFERRALS error:" << ldap_err2string(result);

    qInfo() << "ldap init success, address - ldap://" + mUrl + ":389";
    return LDAP_SUCCESS;
}

// биндинг по логину и паролю
int QLdap::bind() {
    const char *adn;
    adn = (char*)mUsername.toStdString().c_str();
    berval *creds = new berval();
    creds->bv_val = (char*)mPassword.toStdString().c_str();
    creds->bv_len = mPassword.length();
    int result  = ldap_sasl_bind_s(mLDAP,
                                    adn,
                                    LDAP_SASL_SIMPLE,
                                    creds,
                                    nullptr,
                                    nullptr,
                                    nullptr
                                    );
    qDebug() << "DN = " + mUsername;
    if (result != LDAP_SUCCESS)
        qCritical() << "ldap_sasl_bind_s() error:" << ldap_err2string(result);
    else
        qInfo() << "ldap bind success";
    return result;
}

// подключение
int QLdap::connect() {
    readSettings();
    int result = init();
    if (result == LDAP_SUCCESS) {
        return bind();
    }
    return result;
}

// закрытие соединения
int QLdap::close() {
    int result = ldap_unbind_ext_s(mLDAP, nullptr, nullptr);
    if (result != LDAP_SUCCESS)
        qCritical() << "ldap_unbind_ext_s() error:" << ldap_err2string(result);
    return result;
}

// чтение настроек программы
void QLdap::readSettings() {

    QSettings settings(mFilename, QSettings::IniFormat);

    // получение адреса контроллера домена
    QProcess p;
    QString path = "C:/Windows/system32/WindowsPowerShell/v1.0/powershell.exe";
    QStringList commands;
    commands.append("(Get-ADDomainController"
                    " -DomainName $env:USERDNSDOMAIN"
                    " -Discover"
                    " -NextClosestSite).HostName");
    p.start(path, commands);
    p.waitForFinished();
    mUrl = QString(p.readAllStandardOutput()).trimmed();
    //mUrl = "10.82.10.11";
    qDebug() << "Get URL -" << mUrl;

    // чтение основных настроек
    settings.beginGroup("Main");
    mUsername = settings.value("UserDN", "username").toString();
    mPassword = settings.value("Password", "password").toString();
    QString str = settings.value("SearchDN", "People").toString();
    mSearchDNList = str.split('|');
    mCheckingTimeNight = QTime::fromString(settings.value("CheckingTime", "00:00").toString(), "hh:mm");
    if (!mCheckingTimeNight.isValid())
        mCheckingTimeNight.setHMS(0, 0, 0);
    mCheckingTimeDay = mCheckingTimeNight.addSecs(3600 * 12);

    mClientPort = settings.value("ClientPort", 0).toInt();
    settings.endGroup();

    // чтение аттрибутов доступа
    settings.beginGroup("Groups");
    QPair<QString, QStringList> mAttrPair;
    for (int i = 0; i < 15; i++)
    {
        // имя аттрибута доступа
        str = settings
                  .value("Name" + QString::number(i + 1), "empty")
                  .toString();
        if (str.contains("|"))
            str.replace("|", "_");
        mAttrPair.first = str;

        // группы доступа
        str = settings
                  .value("Groups" + QString::number(i + 1), "empty")
                  .toString();
        mAttrPair.second = str.split('|');
        mAttributes[i] = mAttrPair;
    }
    settings.endGroup();
}

// поиск записи
int QLdap::search(const QString &filter,
                  QLdapEntryList *searchResults,
                  const QString &codePage) {

    for(int i = 0; i < mSearchDNList.size(); i++) {

        LDAPMessage *searchResultMsg;

        // запуск поиска
        int result  = ldap_search_ext_s(mLDAP,
                                            (char*)mSearchDNList.at(i).toStdString().c_str(),
                                            LDAP_SCOPE_SUBTREE,
                                            (char*)filter.toStdString().c_str(),
                                            nullptr,
                                            0,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            LDAP_NO_LIMIT,
                                            &searchResultMsg
                                        );

        // ошибка
        if (result != LDAP_SUCCESS) {
            qDebug() << "ldap_parse_result() error:" << ldap_err2string(result);
            return result;
        }

        // обработка результатов
        LDAPMessage *msg;
        for (msg = ldap_first_message(mLDAP, searchResultMsg);
             msg != NULL;
             msg = ldap_next_message(mLDAP, msg)) {

            int msgtype = ldap_msgtype(msg);

            switch(msgtype) {

                // результат поиска
                case LDAP_RES_SEARCH_RESULT:
                {
                    int errcodep = 0;
                    char *error_msg = NULL;

                    // парсинг очередной найденной записи
                    int aresult = ldap_parse_result(mLDAP,
                                                        msg,
                                                        &errcodep,
                                                        nullptr,
                                                        &error_msg,
                                                        nullptr,
                                                        nullptr,
                                                        0
                                                    );

                    // обработки ошибок

                    if (aresult != LDAP_SUCCESS) {
                        qDebug() << "ldap_parse_result() error:" << ldap_err2string(aresult);
                        return aresult;
                    }

                    if (errcodep != LDAP_SUCCESS) {
                        qDebug() << "ldap_parse_result() error:" << error_msg;
                        return errcodep;
                    }

                    if (error_msg) {
                        qDebug() << "ldap_parse_result() error: unknown error";
                        ldap_memfree(error_msg);
                        return errcodep;
                    }

                    break;
                }

                // элемент данных поиска
                case LDAP_RES_SEARCH_ENTRY:
                {
                    BerElement *ber;
                    char *a;

                    QLdapEntry entry;
                    for (a = ldap_first_attribute(mLDAP, msg, &ber);
                         a != NULL;
                         a = ldap_next_attribute(mLDAP, msg, ber)) {

                        struct berval **values;
                        struct berval value;

                        if ((values = ldap_get_values_len(mLDAP, msg, a)) != NULL) {

                            QLdapEntryValues attrvalues = QLdapEntryValues();

                            for (int i = 0; values[i] != NULL; i++) {
                                value = *values[i];
                                QByteArray buf = value.bv_val;
                                QString text;

                                if (codePage.compare("utf-8") == 0)
                                    text = buf;
                                else
                                    text = QString::fromUtf8(buf); //codec->toUnicode(buf);

                                attrvalues.append(text);
                            }

                            entry.insert(a,attrvalues);
                            ldap_value_free_len(values);
                        }

                        ldap_memfree(a);
                    }
                    searchResults->append(entry);
                    break;
                }
            }
        }

        ldap_msgfree(searchResultMsg);
        ldap_msgfree(msg);
    }
    return LDAP_SUCCESS;
}

// поиск по фильтру
int QLdap::userSearch(const QString &filter) {
    mSearchResults->clear();
    QString aFilter = "(&(objectCategory=person)" + filter + ")";
    return search(aFilter, mSearchResults);
}

// поиск по видимому имени
int QLdap::userSearchByName(const QString &name) {
    return this->userSearch("(displayName=" + name + ")");
}

// поиск по логину
int QLdap::userSearchByLogin(const QString &login) {
    return this->userSearch("(sAMAccountName=" + login + ")");
}

// добавление пользователя в группу
int QLdap::addUserToGroup(const QString &username, const QString &groupname) {
    QProcess p;
    QString path = "C:/Windows/system32/WindowsPowerShell/v1.0/powershell.exe";
    QStringList commands;
    QByteArray out;

    qInfo() << "Add user" << username << "to group" << groupname;

    commands.clear();
    commands.append("Add-ADGroupMember -Identity '" +
                    groupname +
                    "' -Members '" +
                    username + "'");

    p.start(path, commands);
    p.waitForFinished();
    out = p.readAllStandardOutput().trimmed();
    if (QString(out).isEmpty())
        return 0;
    else {
        qCritical() << "error adding user '" + username + "' to group '" + groupname + "'";
        return 1;
    }
}

// удаление пользователя из группы
int QLdap::deleteUserFromGroup(const QString &username, const QString &groupname) {
    QProcess p;
    QString path = "C:/Windows/system32/WindowsPowerShell/v1.0/powershell.exe";
    QStringList commands;
    QByteArray out;

    qInfo() << "Delete user" << username << "from" << groupname;

    commands.clear();
    commands.append("Remove-ADGroupMember -Confirm:$false -Identity '" +
                    groupname +
                    "' -Members '" +
                    username + "'");

    p.start(path, commands);
    p.waitForFinished();
    out = p.readAllStandardOutput().trimmed();
    if (QString(out).isEmpty())
        return 0;
    else {
        qCritical() << "error deleting user '" + username + "' from group '" + groupname + "'";
        return 1;
    }
}

// очистка указанного аттрибута пользователя
int QLdap::clearAttribute(const QString &username, const int attr) {
    QProcess p;
    QString path = "C:/Windows/system32/WindowsPowerShell/v1.0/powershell.exe";
    QStringList commands;
    QByteArray out;

    // очистка аттрибутов
    commands.clear();
    commands.append("Set-ADUser -Identity '" +
                    username +
                    "' -Clear extensionAttribute" +
                    QString::number(attr));

    p.start(path, commands);
    p.waitForFinished();
    out = p.readAllStandardOutput();
    if (QString(out.trimmed()).isEmpty())
        return 0;
    else {
        qCritical() << "error clear extensionAttribute" + QString::number(attr) + " for user '" + username + "'";
        return 1;
    }
}

// изменение аттрибута пользователя
int QLdap::setAttributeToUser(const QString &username, const int attr, const QString &value) {
    QProcess p;
    QString path = "C:/Windows/system32/WindowsPowerShell/v1.0/powershell.exe";
    QStringList commands;
    QByteArray out;

    qInfo() << "Set attribute" << getAttributes(attr).first
            << "to value" << value << "for user" << username;

    // редактирование даты
    commands.clear();
    commands.append("Set-ADUser '" +
                    username +
                    "' -Add @{extensionAttribute" +
                    QString::number(attr) +
                    " = '" +
                    value + "'}");

    p.start(path, commands);
    p.waitForFinished();
    out = p.readAllStandardOutput();
    if (QString(out.trimmed()).isEmpty())
        return 0;
    else {
        qCritical() << "error editing extensionAttribute" + QString::number(attr) + " for user '" + username + "'";
        return 1;
    }
}

// сброс пароля
int QLdap::resetPassword(const QString &username) {
    // процесс powershell
    QProcess p;
    QString path = "C:/Windows/system32/WindowsPowerShell/v1.0/powershell.exe";
    QStringList commands;
    QByteArray out;

    // разблокировка пользователя
    commands.clear();
    commands.append("Unlock-ADAccount -Identity '" +
                    username + "'");
    p.start(path, commands);
    p.waitForFinished();
    out = p.readAllStandardOutput().trimmed();

    // успешная разблокировка пользователя - ограничение срока действия пароля
    if (QString(out).isEmpty()) {

        // ограничение срока действия пароля
        qDebug() << "User" << username << "is unlocked";
        commands.clear();
        commands.append("Set-ADUser -Identity '"
                        + username
                        + "' -PasswordNeverExpires:$false");
        p.start(path, commands);
        p.waitForFinished();
        out = p.readAllStandardOutput().trimmed();

        // успешное ограничение срока действия пароля - разрешение на смену пароля
        if (QString(out).isEmpty()) {

            // разрешена смена пароля
            qDebug() << "Editing password's period for user" << username;
            commands.clear();
            commands.append("Set-ADUser -Identity '"
                            + username
                            + "' -CannotChangePassword:$false");
            p.start(path, commands);
            p.waitForFinished();
            out = p.readAllStandardOutput().trimmed();

            // успешное разрешение на смену пароля - сброс пароля
            if (QString(out).isEmpty()) {

                // сброс пароля
                qDebug() << "Access editing password for user" << username;
                commands.clear();
                commands.append("Set-ADAccountPassword '" + username +
                                "' -Reset -NewPassword "
                                "(ConvertTo-SecureString -AsPlainText 'Qq123123' "
                                "-Force -Verbose)");
                p.start(path, commands);
                p.waitForFinished();
                out = p.readAllStandardOutput().trimmed();

                // успешный сброс пароля - требование смены пароля
                if (QString(out).isEmpty()) {

                    // требование смены пароля
                    qDebug() << "Reset password for user" << username;
                    commands.clear();
                    commands.append("Set-ADuser -Identity '"
                                    + username
                                    + "' -ChangePasswordAtLogon:$true");

                    p.start(path, commands);
                    p.waitForFinished();
                    out = p.readAllStandardOutput().trimmed();
                }

                // завершение сброса пароля
                if (QString(out).isEmpty()) {
                    qDebug() << "Requirement to edit default password for user" << username;
                    qInfo() << "Complete reset password for user" << username;
                    return 0;
                }
                else {
                    qCritical() << "Error requirement to edit default password for user"
                                << username;
                    return 1;
                }
            }
            else {
                qCritical() << "Error access editing password for user" << username;
                return 2;
            }
        }
        else {
            qCritical() << "Error editing password's period for user" << username;
            return 3;
        }
    }
    else {
        qCritical() << "Error unlock user" << username;
        return 4;
    }
}

// получение результатов поиска
QLdapEntryList *QLdap::getSearchResult() const {
    return mSearchResults;
}

// инициализация результата поиска, указатель передается извне
void QLdap::setSearchResult(QLdapEntryList *value) {
    mSearchResults = value;
}

// получение логина
QString QLdap::getLogin() const {
    return mUsername;
}

// получение пароля
QString QLdap::getPassword() const {
    return mPassword;
}

// получение ночного времени проверки доступа
QTime QLdap::getCheckingTimeNight() const {
    return mCheckingTimeNight;
}

// получение дневного времени проверки доступа
QTime QLdap::getCheckingTimeDay() const {
    return mCheckingTimeDay;
}

// получение порта для подключения
int QLdap::getClientPort() const {
    return mClientPort;
}

// получение пары атрибут - группы
QPair<QString, QStringList> QLdap::getAttributes(const int index) const {
    return mAttributes[index - 1];
}

}
