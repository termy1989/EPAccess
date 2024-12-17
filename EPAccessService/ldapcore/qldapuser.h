#ifndef QLDAPUSER_H
#define QLDAPUSER_H

#include "qldap.h"
#include <QTextStream>
#include <QDebug>

namespace ldapcore {

// класс пользователя LDAP
class QLdapUser
{

public:
    QLdapUser(QLdapEntry *user);                                        // конструктор
    QString getUserValue(const QString&) const;                         // получение данных пользователя
    QString getDisplayName() const;                                     // имя пользователя
    QString getDistinguishedName() const;                               // полное имя-путь пользователя
    QString getSamAccountName() const;                                  // логин пользователя
    QString getMemberOf() const;                                        // членство в группах
    QString getAttribute(const int) const;                              // аттрибуты доступа
    QString getAttribute(const int, const QString&) const;              // аттрибуты доступа
    QString getAttribute(const int, const QStringList&) const;          // аттрибуты доступа
    QString toString() const;                                           // пребразование данных пользователя в строку

    // перегрузки операторов << для отладки (вывода)
    friend QDebug operator<<(QDebug debug, const QLdapUser &user);
    friend QDebug operator<<(QDebug debug, const QLdapUser *user);

private:
    QLdapEntry *user = nullptr;                                         // экземпляр пользователя
};

}
#endif // QLDAPUSER_H
