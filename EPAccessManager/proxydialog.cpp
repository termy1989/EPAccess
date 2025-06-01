#include "proxydialog.h"
#include "ui_proxydialog.h"

// конструктор
ProxyDialog::ProxyDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProxyDialog)
{
    ui->setupUi(this);

    // установка параметров диалогового окна
    this->setFixedSize(370, 200);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(SendInfo()));
    connect(ui->checkBox_proxy_off, SIGNAL(clicked(bool)), this, SLOT(proxyOff(bool)));

    // табуляция по элементам
    this->setTabOrder(ui->lineEdit_url, ui->spinBox_port);
    this->setTabOrder(ui->spinBox_port, ui->lineEdit_login);
    this->setTabOrder(ui->lineEdit_login, ui->lineEdit_password);
    this->setTabOrder(ui->lineEdit_password, ui->radioButton_http);
    this->setTabOrder(ui->radioButton_http, ui->radioButton_httpcaching);
    this->setTabOrder(ui->radioButton_httpcaching, ui->radioButton_socks5);
    this->setTabOrder(ui->radioButton_socks5, ui->checkBox_proxy_off);
    this->setTabOrder(ui->checkBox_proxy_off, ui->buttonBox);
}

// деструктор
ProxyDialog::~ProxyDialog()
{
    delete ui;
}

// установка настроек прокси
void ProxyDialog::openSettings(const Proxy &proxy)
{
    mProxy = proxy;

    if (mProxy.type == 0) {
        ui->lineEdit_url->clear();
        ui->lineEdit_login->clear();
        ui->lineEdit_password->clear();
        ui->spinBox_port->setValue(0);
        ui->lineEdit_url->setEnabled(false);
        ui->lineEdit_login->setEnabled(false);
        ui->lineEdit_password->setEnabled(false);
        ui->spinBox_port->setEnabled(false);
        ui->radioButton_http->setEnabled(false);
        ui->radioButton_httpcaching->setEnabled(false);
        ui->radioButton_socks5->setEnabled(false);
        ui->checkBox_proxy_off->setChecked(true);
    }
    else {
        if (mProxy.url != "")
            ui->lineEdit_url->setText(QString::fromStdString(mProxy.url));
        else
            ui->lineEdit_url->clear();
        if (mProxy.login != "")
            ui->lineEdit_login->setText(QString::fromStdString(mProxy.login));
        else
            ui->lineEdit_login->clear();
        if (mProxy.password != "")
            ui->lineEdit_password->setText(QString::fromStdString(mProxy.password));
        else
            ui->lineEdit_password->clear();

        ui->spinBox_port->setValue(mProxy.port);

        if (mProxy.type == 1) {
            ui->radioButton_http->setChecked(true);
            ui->radioButton_httpcaching->setChecked(false);
            ui->radioButton_socks5->setChecked(false);
        }
        else if (mProxy.type == 2) {
            ui->radioButton_http->setChecked(false);
            ui->radioButton_httpcaching->setChecked(true);
            ui->radioButton_socks5->setChecked(false);
        }
        else if (mProxy.type == 3) {
            ui->radioButton_http->setChecked(false);
            ui->radioButton_httpcaching->setChecked(false);
            ui->radioButton_socks5->setChecked(true);
        }
    }

    show();

}

// отправка настроек прокси
void ProxyDialog::SendInfo()
{
    if (ui->checkBox_proxy_off->isChecked()) {
        mProxy.type = 0;
    }
    else {
        if (ui->radioButton_http->isChecked())
            mProxy.type = 1;
        else if (ui->radioButton_httpcaching->isChecked())
            mProxy.type = 2;
        else if (ui->radioButton_socks5->isChecked())
            mProxy.type = 3;

        mProxy.url = ui->lineEdit_url->text().toStdString();
        mProxy.port = ui->spinBox_port->value();
        mProxy.login = ui->lineEdit_login->text().toStdString();
        mProxy.password = ui->lineEdit_password->text().toStdString();
    }
    emit signalOk(mProxy);
}

// обработчик галочки "без прокси"
void ProxyDialog::proxyOff(bool check)
{
    if (check) {
        ui->lineEdit_url->clear();
        ui->lineEdit_login->clear();
        ui->lineEdit_password->clear();
        ui->spinBox_port->setValue(0);
        ui->lineEdit_url->setEnabled(false);
        ui->lineEdit_login->setEnabled(false);
        ui->lineEdit_password->setEnabled(false);
        ui->spinBox_port->setEnabled(false);
        ui->radioButton_http->setEnabled(false);
        ui->radioButton_httpcaching->setEnabled(false);
        ui->radioButton_socks5->setEnabled(false);
    }
    else {
        ui->lineEdit_url->setEnabled(true);
        ui->lineEdit_login->setEnabled(true);
        ui->lineEdit_password->setEnabled(true);
        ui->spinBox_port->setEnabled(true);
        ui->radioButton_http->setEnabled(true);
        ui->radioButton_httpcaching->setEnabled(true);
        ui->radioButton_socks5->setEnabled(true);
    }
}
