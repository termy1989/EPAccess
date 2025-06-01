#ifndef PROXYDIALOG_H
#define PROXYDIALOG_H

#include <QDialog>

namespace Ui {
class ProxyDialog;
}

// структура прокси
struct Proxy {
    uint8_t type;                                       // тип прокси
    std::string url;                                    // адрес сервера
    uint16_t port;                                      // порт
    std::string login;                                  // логин
    std::string password;                               // пароль
};

// класс окна настройки прокси-соединения
class ProxyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProxyDialog(
            QWidget *parent = nullptr
        );                                              // конструктор
    ~ProxyDialog();                                     // деструктор
    void openSettings(const Proxy&);                    // установка настроек прокси

signals:
    void signalOk(const Proxy&);                        // отправка сигнала со структурой авторизации

private slots:
    void SendInfo();                                    // обработчик нажатия ОК
    void proxyOff(bool);                                // обработчик галочки "без прокси"

private:
    Ui::ProxyDialog *ui;                                // экземпляр диалогового окна настройки прокси
    Proxy mProxy;                                       // экземпляр структуры прокси
};

#endif // PROXYDIALOG_H
