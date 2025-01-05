#include "accessdialog.h"
#include "ui_accessdialog.h"

// конструктор
AccessDialog::AccessDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AccessDialog) {
    ui->setupUi(this);
    this->setFixedSize(300, 130);
    ui->dateEdit_access->setDate(QDate::currentDate());
    connect(ui->buttonBox_main, SIGNAL(accepted()), this, SLOT(slotSendInfo()));
    connect(ui->checkBox_vip, SIGNAL(clicked(bool)), this, SLOT(slotCheckBoxClicked(bool)));
}

// деструктор
AccessDialog::~AccessDialog() {
    delete ui;
}

// установка списка аттрибутов
void AccessDialog::slotSetAttributes(const QStringList &list) {

    QStringList itemList;
    QString str = "";

    // составление списка аттрибутов (первый элемент лишний)
    for (int i = 0; i < list.size(); i++) {
        str = QString::number(i)
                         + " - "
                         + list.at(i);
        if (list.at(i) != "empty")
            itemList.push_back(str);
    }
    itemList.removeAt(0);

    // обновление combobox
    ui->comboBox_access->clear();
    if (!itemList.isEmpty()) {
        ui->comboBox_access->addItems(itemList);
        ui->comboBox_access->setCurrentIndex(0);
    }
    else
        this->setEnabled(false);
}

// отправка настроек доступа основному окну
void AccessDialog::slotSendInfo() {

    // установка постоянного доступа
    if (ui->checkBox_vip->isChecked())
        emit signalOk(ui->comboBox_access->currentText()
                                                .split(" ")
                                                .at(0)
                                                .toInt(), "vip");

    else {

        // проверка установленной даты на корректность
        if (ui->dateEdit_access->date() <= QDate::currentDate()) {
            QMessageBox::critical(this, "Ошибка", "Введите корректную дату!");
            this->show();
        }
        else

            // отправка настроек
            emit signalOk(ui->comboBox_access->currentText()
                                                    .split(" ")
                                                    .at(0)
                                                    .toInt(),
                                    ui->dateEdit_access->date()
                                            .toString("dd.MM.yy"));
    }
}

// нажатие отметки постоянного доступа
void AccessDialog::slotCheckBoxClicked(bool clicked) {
    if (clicked)
        ui->dateEdit_access->setEnabled(false);
    else
        ui->dateEdit_access->setEnabled(true);
}

// нажатие на кнопку отключение доступа
void AccessDialog::on_pushButton_off_clicked() {
    if (QMessageBox::warning(nullptr, tr("Предупреждение"),
                             tr("Вы уверены, что хотите отключить "
                                "доступ у выбранных пользователей?"),
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

        reject();
        emit signalDel(ui->comboBox_access->currentText()
                                                .split(" ")
                                                .at(0)
                                                .toInt());
    }
}

