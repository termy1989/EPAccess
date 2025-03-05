#include "logindialog.h"
#include "ui_logindialog.h"

// конструктор
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog) {
    ui->setupUi(this);

    // инициализация диалогового окна настройки прокси
    mProxyDialog = new ProxyDialog(this);

    // обнуление настроек прокси
    mAuth.proxy.type = 0;

    // установка параметров диалогового окна

    // установка параметров диалогового окна
    this->setFixedSize(325, 155);
    #ifdef Q_OS_LINUX
    this->setFixedSize(395, 165);
    #endif

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(SendInfo()));
    connect(mProxyDialog, SIGNAL(signalOk(const Proxy&)),
                this, SLOT(setProxy(const Proxy&)));

    // табуляция по элементам
    this->setTabOrder(ui->lineEdit_url, ui->lineEdit_login);
    this->setTabOrder(ui->lineEdit_login, ui->lineEdit_password);
    this->setTabOrder(ui->lineEdit_password, ui->checkBox_save);
    this->setTabOrder(ui->checkBox_save, ui->proxyButton);
    this->setTabOrder(ui->proxyButton, ui->buttonBox);

    QString path = "";
    path = QDir::homePath() + PATH_WIN;
    #ifdef Q_OS_LINUX
    path = QDir::homePath() + PATH_LINUX;
    #endif

    // проверка наличия (создание) директории и сохраненной авторизации
    if (createLocalDirectory()) {
        QFile file(path);

        // загрузка авторизации, если файл есть
        if (file.exists())
            loadAuth();
    }
}

// деструктор
LoginDialog::~LoginDialog() {
    delete mProxyDialog;
    delete ui;
}

// сохранение данных авторизации в файл
void LoginDialog::saveAuth() {

    QString path = "";
    path = QDir::homePath() + PATH_WIN;
    #ifdef Q_OS_LINUX
    path = QDir::homePath() + PATH_LINUX;
    #endif

    // открытие бинарника на запись
    std::fstream fs;
    fs.open(path.toStdString().c_str(),
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

    // сохранение статуса прокси
    fs.write((char*) &mAuth.proxy.type, sizeof(uint8_t));

    if (mAuth.proxy.type > 0) {

        // сохранение адреса прокси
        len = mAuth.proxy.url.length();
        fs.write((char*) &len, sizeof(size_t));
        if (len > 0)
            fs.write(mAuth.proxy.url.data(), len);

        // сохранение порта прокси
        fs.write((char*) &mAuth.proxy.port, sizeof(uint16_t));

        // сохранение логина прокси
        len = mAuth.proxy.login.length();
        fs.write((char*) &len, sizeof(size_t));
        if (len > 0)
            fs.write(mAuth.proxy.login.data(), len);

        // сохранение пароля прокси
        len = mAuth.proxy.password.length();
        fs.write((char*) &len, sizeof(size_t));
        if (len > 0)
            fs.write(mAuth.proxy.password.data(), len);
    }

    // закрытие бинарника
    fs.close();
}

// загрузка данных авторизации из файла
void LoginDialog::loadAuth() {

    QString path = "";
    path = QDir::homePath() + PATH_WIN;
    #ifdef Q_OS_LINUX
    path = QDir::homePath() + PATH_LINUX;
    #endif

    // открытие бинарника для чтения
    size_t len;
    std::fstream fs;
    fs.open(path.toStdString().c_str(),
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

    // чтение статуса прокси
    fs.read((char*) &mAuth.proxy.type, sizeof(uint8_t));

    if (mAuth.proxy.type != 0) {

        // чтение адреса прокси
        fs.read((char*) &len, sizeof(size_t));
        if (len > 0) {
            char buf3[len+1];
            fs.read(buf3, len);
            buf3[len] = 0;
            mAuth.proxy.url = buf3;
        }
        else
            mAuth.proxy.url = "";

        // чтение порта
        fs.read((char*) &mAuth.proxy.port, sizeof(uint16_t));

        // чтение логина
        fs.read((char*) &len, sizeof(size_t));
        if (len > 0) {
            char buf3[len+1];
            fs.read(buf3, len);
            buf3[len] = 0;
            mAuth.proxy.login = buf3;
        }
        else
            mAuth.proxy.login = "";

        // чтение пароля
        fs.read((char*) &len, sizeof(size_t));
        if (len > 0) {
            char buf3[len+1];
            fs.read(buf3, len);
            buf3[len] = 0;
            mAuth.proxy.password = buf3;
        }
        else
            mAuth.proxy.password = "";
    }

    // заполнение полей авторизации загруженными данными
    ui->lineEdit_url->setText(QString::fromStdString(mAuth.url));
    ui->lineEdit_login->setText(QString::fromStdString(mAuth.login));
    ui->lineEdit_password->setText(QString::fromStdString(mAuth.password));

    // закрытие бинарника
    fs.close();
}

// создание директории сохранения авторизации
bool LoginDialog::createLocalDirectory() {

    QString path = "";
    path = QDir::homePath() + "/AppData/Local/EPAccessManager";
    #ifdef Q_OS_LINUX
    path = QDir::homePath() + "/.local/share/EPAccessManager";
    #endif
    QDir dir(path);
    if (!dir.exists())
        return dir.mkdir(path);
    else return true;
}

// отправка сигнала с данными авторизации основному окну
void LoginDialog::SendInfo()
{
    // проверка, все ли поля заполнены
    if (ui->lineEdit_url->text().isEmpty()
        || ui->lineEdit_password->text().isEmpty()
        || ui->lineEdit_login->text().isEmpty()) {
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

// установка настроек прокси
void LoginDialog::setProxy(const Proxy &proxy)
{
    mAuth.proxy = proxy;
}

// открытие диалогового окна настройки прокси-соединения
void LoginDialog::on_proxyButton_clicked()
{
    mProxyDialog->openSettings(mAuth.proxy);
}

