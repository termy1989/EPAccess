#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
class ProgressDialog;
}

// класс диалогового окна прогресса
class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = nullptr);     // конструктор
    ~ProgressDialog();                                      // деструктор

public slots:
    void slotUpdateStatus(quint16, quint16,
                          QString, QString);                // обновление статуса работы

private:
    Ui::ProgressDialog *ui = nullptr;                       // экземпляр окна
    bool isStarted = false;                                 // статус работы окна
};

#endif // PROGRESSDIALOG_H
