#include "progressdialog.h"
#include "ui_progressdialog.h"

// конструктор
ProgressDialog::ProgressDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProgressDialog) {
    ui->setupUi(this);
    connect(this, &ProgressDialog::rejected, this, [=](){if(isStarted) show();});
}

// деструктор
ProgressDialog::~ProgressDialog() {
    delete ui;
}

// обновление статуса работы
void ProgressDialog::slotUpdateStatus(quint16 progress,
                                        quint16 sum,
                                        const QString &user,
                                        const QString &stage) {

    // запуск окна
    if (!isStarted) {
        ui->progressBar->setMinimum(0);
        ui->progressBar->setValue(0);
        isStarted = true;
    }

    // обновление статуса
    this->show();
    ui->progressBar->setMaximum(sum);
    ui->progressBar->setValue(progress);
    ui->label_user->setText(user);
    ui->label_action->setText(stage);

    // закрытие окна
    if (ui->progressBar->value() == sum) {
        isStarted = false;
        this->close();
    }
}
