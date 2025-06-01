#include "qldapuser.h"

namespace ldapcore {

// конструктор
QLdapUser::QLdapUser(QLdapEntry *user) {
    this->user = user;
}

// получение данных пользователя
QString QLdapUser::getUserValue(const QString &key) const {
    return (*this->user)[key].join(",");
}

// имя пользователя
QString QLdapUser::getDisplayName() const {
    return this->getUserValue("displayName");
}

QString QLdapUser::getDistinguishedName() const {
    return this->getUserValue("distinguishedName");
}

// логин пользователя
QString QLdapUser::getSamAccountName() const {
    return this->getUserValue("sAMAccountName");
}

// членство в группах
QString QLdapUser::getMemberOf() const {
    return this->getUserValue("memberOf");
}

// аттрибуты доступа
QString QLdapUser::getAttribute(const int index) const {
    return this->getUserValue("extensionAttribute"
                              + QString::number(index));
}

// аттрибуты доступа
QString QLdapUser::getAttribute(const int index, const QString &group) const {
    if (this->getMemberOf().contains(group))
        return this->getUserValue("extensionAttribute"
                                  + QString::number(index));
    else return "";
}

// аттрибуты доступа
QString QLdapUser::getAttribute(const int index, const QStringList &groups) const {
    bool memberOf = true;
    for (int i = 0; i < groups.size(); i++) {
        if (!this->getMemberOf().contains(groups.at(i)))
            memberOf = false;
    }
    if (memberOf)
        return this->getUserValue("extensionAttribute"
                                  + QString::number(index));
    else return "";
}

// пребразование данных пользователя в строку
QString QLdapUser::toString() const {
    QString s;
    QTextStream out(&s);

    out << "QLdapUser( "
        << "displayName: " << this->getDisplayName() << ","
        << "samAccountName: " << this->getSamAccountName() << ","
        << "memberOf: " << this->getMemberOf() << " )"
        ;
    return s;
}

QDebug operator<<(QDebug debug, const QLdapUser &user) {
    QDebugStateSaver saver(debug);

    debug.nospace() << "QLdapUser( "
                    << "displayName: " << user.getDisplayName() << ", "
                    << "samAccountName: " << user.getSamAccountName() << ", "
                    << "memberOf: " << user.getMemberOf()
                    << " )";
    return debug;
}

QDebug operator<<(QDebug debug, const QLdapUser *user) {
    QDebugStateSaver saver(debug);
    debug.nospace() << *user;
    return debug;
}

}
