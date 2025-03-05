#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QString>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <fstream>
#include "proxydialog.h"
#define PATH_WIN "/AppData/Local/EPAccessManager/auth.dat"
#define PATH_LINUX "/.local/share/EPAccessManager/auth.dat"

namespace Ui {
class LoginDialog;
}

// структура авторизации
struct Auth {
    std::string url;                                    // адрес сервера
    std::string login;                                  // логин
    std::string password;                               // пароль
    Proxy proxy;                                        // настройка прокси
};

// класс окна авторизации
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);    // конструктор
    ~LoginDialog();                                     // деструктор
    void saveAuth();                                    // сохранение структуры авторизации в файл
    void loadAuth();                                    // загрузка структуры авторизации из файла
    bool createLocalDirectory();                        // создание каталога хранения структуры авторизации

signals:
    void signalOk(const Auth&);                         // отправка сигнала со структурой авторизации

private slots:
    void SendInfo();                                    // обработчик нажатия ОК
    void setProxy(const Proxy&);                        // установка настроек прокси
    void on_proxyButton_clicked();                      // обработчик нажатия на кнопку настройки прокси-соединения

private:
    Ui::LoginDialog *ui = nullptr;                      // экземпляр диалогового окна авторизации
    ProxyDialog *mProxyDialog = nullptr;                // экземпляр диалогового окна настройки прокси
    Auth mAuth;                                         // экземпляр структуры авторизации
};

#endif // LOGINDIALOG_H
