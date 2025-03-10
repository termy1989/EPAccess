#ifndef QLDAP_H
#define QLDAP_H

#include <QString>
#include <QTime>
#include <QDebug>
#include <QObject>
#include <QTextStream>
#include <QProcess>
#include <QSettings>

#include "ldap.h"

namespace ldapcore {

typedef QStringList QLdapEntryValues;                                       // список "носителей" свойств
typedef QHash<QString, QLdapEntryValues> QLdapEntry;                        // пользователь (свойство - значение)
typedef QList<QLdapEntry> QLdapEntryList;                                   // список свойств

// основной класс для работы с LDAP
class QLdap
{
public:
    QLdap(QString);                                                         // конструктор
    ~QLdap();                                                               // деструктор
    int init();                                                             // инициализация соединения
    int bind();                                                             // биндинг по логину
    int connect();                                                          // подключение
    int close();                                                            // закрытие соединения
    void readSettings();                                                    // чтение настроек программы
    int search(const QString &filter,
                   QLdapEntryList *searchResults,
                   const QString &codePage="utf-8"
                   );                                                       // поиск записи
    int userSearch(const QString &filter);                                  // поиск по указанному фильтру
    int userSearchByName(const QString &name);                              // поиск по видимому имени
    int userSearchByLogin(const QString &login);                            // поиск по логину
    int addUserToGroup(const QString &username,
                       const QString &groupname);                           // добавление пользователя в группу
    int deleteUserFromGroup(const QString &username,
                            const QString &groupname);                      // удаление пользователя из группы
    int clearAttribute(const QString &username,
                            const int attr);                                // очистка указанного аттрибута пользователя
    int setAttributeToUser(const QString &username,
                           const int attr,
                           const QString &value);                           // установка указанного аттрибута пользователя
    int resetPassword(const QString &username);                             // сброс пароля
    QLdapEntryList* getSearchResult() const;                                // получение результатов поиска
    void setSearchResult(QLdapEntryList *value);                            // установка контейнера для результатов поиска
    QString getLogin() const;                                               // получение логина из конфига
    QString getPassword() const;                                            // получение пароля из конфига
    //QString getCheckingTime() const;                                        // получение времени проверки доступа
    QTime getCheckingTimeNight() const;                                     // получение ночного времени проверки доступа
    QTime getCheckingTimeDay() const;                                       // получение дневного времени проверки доступа
    int getClientPort() const;                                              // получение порта для подключения
    QPair<QString, QStringList> getAttributes(const int index) const;       // получение пары атрибут - группы

private:
    LDAP *mLDAP = nullptr;                                                  // LDAP-объект
    QString mFilename = "";                                                 // имя конфиг. файла
    QLdapEntryList *mSearchResults = nullptr;                               // список найденных пользователей
    QString mUrl = "";                                                      // адрес контроллера домена
    QString mUsername = "";                                                 // имя доменного пользователя (админа)
    QString mPassword = "";                                                 // пароль
    QStringList mSearchDNList;                                              // список областей поиска пользователя
    QTime mCheckingTimeNight;                                               // дневное время проверки доступа
    QTime mCheckingTimeDay;                                                 // ночное время проверки доступа
    int mClientPort = 0;                                                    // порт для подключения мджера
    QPair<QString, QStringList> mAttributes[15];                            // пара атрибут - группы
};

}
#endif // QLDAP_H
