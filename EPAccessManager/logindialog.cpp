#include "logindialog.h"
#include "ui_logindialog.h"

// конструктор
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog) {
    ui->setupUi(this);

    // установка параметров диалогового окна
    this->setFixedSize(325, 155);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(SendInfo()));

    // проверка наличия (создание) директории и сохраненной авторизации
    if (createLocalDirectory()) {
        QFile file(QDir::homePath() + "/AppData/Local/EPAccessManager/auth.dat");

        // загрузка авторизации, если файл есть
        if (file.exists())
            loadAuth();
    }
}

// деструктор
LoginDialog::~LoginDialog() {
    delete ui;
}

// сохранение данных авторизации в файл
void LoginDialog::saveAuth() {

    // открытие бинарника на запись
    std::fstream fs;
    fs.open((QDir::homePath() + PATH).toStdString().c_str(),
                        std::fstream::out | std::fstream::binary);

    // сохранение url
    size_t len = mAuth.url.length();
    fs.write((char*) &len, sizeof(size_t));
    fs.write(mAuth.url.data(), len);

    // сохранение логина
    len = mAuth.login.length();
    fs.write((char*) &len, sizeof(size_t));
    fs.write(mAuth.login.data(), len);

    // сохранение пароля
    len = mAuth.password.length();
    fs.write((char*) &len, sizeof(size_t));
    fs.write(mAuth.password.data(), len);

    // закрытие бинарника
    fs.close();
}

// загрузка данных авторизации из файла
void LoginDialog::loadAuth() {

    // открытие бинарника для чтения
    size_t len;
    std::fstream fs;
    fs.open((QDir::homePath() + PATH).toStdString().c_str(),
                        std::fstream::in | std::fstream::binary);

    // чтение url
    fs.read((char*)&len, sizeof(size_t));
    char buf1[len+1];
    fs.read(buf1, len);
    buf1[len] = 0;
    mAuth.url = buf1;

    // чтение логина
    fs.read((char*) &len, sizeof(size_t));
    char buf2[len+1];
    fs.read(buf2, len);
    buf2[len] = 0;
    mAuth.login = buf2;

    // чтение пароля
    fs.read((char*) &len, sizeof(size_t));
    char buf3[len+1];
    fs.read(buf3, len);
    buf3[len] = 0;
    mAuth.password = buf3;

    // заполнение полей авторизации загруженными данными
    ui->lineEdit_url->setText(QString::fromStdString(mAuth.url));
    ui->lineEdit_login->setText(QString::fromStdString(mAuth.login));
    ui->lineEdit_password->setText(QString::fromStdString(mAuth.password));

    // закрытие бинарника
    fs.close();
}

// создание директории сохранения авторизации
bool LoginDialog::createLocalDirectory() {
    QDir dir(QDir::homePath()
             + "/AppData/Local/EPAccessManager");
    if (!dir.exists())
        return dir.mkdir(QDir::homePath()
                         + "/AppData/Local/EPAccessManager");
    else return true;
}

// отправка сигнала с данными авторизации основному окну
void LoginDialog::SendInfo()
{
    // проверка, все ли поля заполнены
    if (ui->lineEdit_url->text().isEmpty()
        || ui->lineEdit_login->text().isEmpty()
        || ui->lineEdit_password->text().isEmpty()) {
        QMessageBox::critical(this, "Ошибка авторизации",
                              "Заполните все поля!");
        show();
    }
    else {
        mAuth.url = ui->lineEdit_url->text().toStdString();
        mAuth.login = ui->lineEdit_login->text().toStdString();
        mAuth.password = ui->lineEdit_password->text().toStdString();

        // сохранение структуры, если галочка
        if (ui->checkBox_save->isChecked())
            saveAuth();
        emit signalOk(mAuth);
    }

}
