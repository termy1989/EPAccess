#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include "tcphandler.h"
#include "logindialog.h"
#include "accessdialog.h"
#include "progressdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

// класс основного окна приложения
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);          // конструктор
    ~MainWindow();                                  // деструктор

private slots:
    void slotClickButton(bool);                     // нажатие на кнопки доступа и сброса
    void slotAuth(const Auth&);                     // авторизация на сервере
    void slotUpdateUserList(QStandardItemModel*);   // обновление списка пользователей
    void slotEditAccess(quint8, const QString&);    // отправка на сервер настроек доступа
    void slotDelAccess(quint8);                     // отправка запроса на отключение доступа
    void slotOperationReady(bool);                  // завершение операции с доступом или паролем
    void slotConnectError(const QString&);          // обработка ошибки соединения

    // обработчики нажатия кнопок
    void on_pushButton_search_clicked();
    void on_pushButton_access_clicked();
    void on_pushButton_reset_clicked();

private:
    Ui::MainWindow *ui = nullptr;                   // экземпляр основного окна
    LoginDialog *mLoginDialog = nullptr;            // экземпляр диалогового окна авторизации
    AccessDialog *mAccessDialog = nullptr;          // экземпляр диалогового окна установки доступа
    ProgressDialog *mProgressDialog = nullptr;      // экземпляр диалогового окна прогресса
    TCPhandler *mTCPhandler = nullptr;              // экземпляр обработчика TCP
};
#endif // MAINWINDOW_H
