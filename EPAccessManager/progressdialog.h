#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = nullptr);
    ~ProgressDialog();

public slots:
    void slotUpdateStatus(int, int, QString, QString);          // обновление статуса работы

private:
    Ui::ProgressDialog *ui;
    bool isStarted = false;                                 // статус работы окна
};

#endif // PROGRESSDIALOG_H
