#ifndef ACCESSDIALOG_H
#define ACCESSDIALOG_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class AccessDialog;
}

// класс диалогового окна установки доступа
class AccessDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AccessDialog(QWidget *parent = nullptr);       // конструктор
    ~AccessDialog();                                        // деструктор

signals:
    void signalOk(quint8, QString);                         // отправка сигнала с установленной датой
    void signalDel(quint8);                                 // отправка сигнала об отключении доступа

private slots:
    void slotSetAttributes(const QStringList&);             // установка списка аттрибутов
    void slotSendInfo();                                    // обработчик нажатия OK
    void slotCheckBoxClicked(bool);                         // отметка постоянного доступа
    void on_pushButton_off_clicked();                       // кнопка отключения доступа

private:
    Ui::AccessDialog *ui = nullptr;                         // экземпляр диалогового окна
};

#endif // ACCESSDIALOG_H
