#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QString>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <fstream>
#define PATH "/AppData/Local/EPAccessManager/auth.dat"

namespace Ui {
class LoginDialog;
}

// структура авторизации
struct Auth {
    std::string url;                                    // адрес сервера
    std::string login;                                  // логин
    std::string password;                               // пароль
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
    void signalOk(Auth);                                // отправка сигнала со структурой авторизации

private slots:
    void SendInfo();                                    // обработчик нажатия ОК

private:
    Ui::LoginDialog *ui = nullptr;                      // экземпляр диалогового окна
    Auth mAuth;                                         // экземпляр структуры авторизации
};

#endif // LOGINDIALOG_H
