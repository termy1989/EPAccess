#include "mainwindow.h"
#include "ui_mainwindow.h"

// конструктор
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // блокировка и растяжение окна
    QWidget::showMaximized();
    ui->centralwidget->setEnabled(false);
    ui->statusbar->showMessage("Отключено");

    // обработка нажатия Enter
    connect(ui->lineEdit_search, SIGNAL(returnPressed()),
                                ui->pushButton_search, SIGNAL(clicked()));

    // инициализация обработчика ТСР
    mTCPhandler = new TCPhandler();
    connect(mTCPhandler, SIGNAL(signalAuthSuccess()),
                        this, SLOT(on_pushButton_search_clicked()));
    connect(mTCPhandler, SIGNAL(signalUpdateUserList(QStandardItemModel*)),
                        this, SLOT(slotUpdateUserList(QStandardItemModel*)));
    connect(mTCPhandler, SIGNAL(signalOperationReady(bool)),
                        this, SLOT(slotOperationReady(bool)));
    connect(mTCPhandler, SIGNAL(signalConnectError(const QString&)),
                        this, SLOT(slotConnectError(const QString&)));

    // инициализация окна авторизации
    mLoginDialog = new LoginDialog(this);
    connect(mLoginDialog, &QDialog::rejected, this, [=](){exit(0);});
    connect(mLoginDialog, SIGNAL(signalOk(const Auth&)), this, SLOT(slotAuth(const Auth&)));

    // инициализация окна установки доступа
    mAccessDialog = new AccessDialog(this);
    connect(mAccessDialog, &QDialog::accepted,
                        this, [=](){ui->centralwidget->setEnabled(true);});
    connect(mAccessDialog, &QDialog::rejected,
                        this, [=](){ui->centralwidget->setEnabled(true);});
    connect(mAccessDialog, SIGNAL(signalOk(quint8,const QString&)),
                        this, SLOT(slotEditAccess(quint8,const QString&)));
    connect(mAccessDialog, SIGNAL(signalDel(quint8)),
                        this, SLOT(slotDelAccess(quint8)));
    connect(mTCPhandler, SIGNAL(signalUpdateAttributes(const QStringList&)),
                        mAccessDialog, SLOT(slotSetAttributes(const QStringList&)));

    // инициализация окна прогресса
    mProgressDialog = new ProgressDialog(this);
    connect(mTCPhandler,
            SIGNAL(signalUpdateProgress(quint16,quint16,const QString&,const QString&)),
                mProgressDialog,
            SLOT(slotUpdateStatus(quint16,quint16,const QString&,const QString&)));
    connect(mTCPhandler, &TCPhandler::signalUpdateProgress,
            this, [=](){ui->centralwidget->setEnabled(false);});

    // открытие окна авторизации при первом запуске
    mLoginDialog->show();
}

// деструктор
MainWindow::~MainWindow() {
    if (mTCPhandler != nullptr)
        delete mTCPhandler;
    delete mLoginDialog;
    delete mAccessDialog;
    delete mProgressDialog;
    delete ui;
}

// нажатие на кнопки доступа и сброса
void MainWindow::slotClickButton(bool access)
{
    // выбранные пользователи
    QModelIndexList indexes = ui->tableView_users
                                  ->selectionModel()
                                  ->selection().indexes();

    // ни один пользователь не выбран
    if (indexes.count() == 0)
        QMessageBox::critical(nullptr,
                              tr("Ошибка"),
                              tr("Выберите хотя бы одного пользователя!"));
    else
    {
        // блокировка основного окна
        ui->centralwidget->setEnabled(false);

        // сброс пароля
        if (!access)
        {
            if (QMessageBox::warning(nullptr, tr("Предупреждение"),
                                     tr("Вы уверены, что хотите сбросить "
                                        "пароли у выбранных пользователей?"),
                                     QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {

                // цикл по выделенным ячейкам таблицы
                QStringList users;
                for (int i = 0; i < indexes.count(); ++i) {

                    // составление списка имен пользователей на сброс
                    if (indexes.at(i).column() == 0)
                        users.append(indexes.at(i)
                                         .siblingAtColumn(0)
                                         .data()
                                         .toString());
                }

                // отправка списка пользователей на сервер
                mTCPhandler->sendResetReq(users);
            }
            else ui->centralwidget->setEnabled(true);
        }

        // редактирование доступа
        else
            mAccessDialog->show();
    }
}

// авторизация на сервере
void MainWindow::slotAuth(const Auth &auth) {
    mTCPhandler->authOnServer(auth);
}

// обновление списка пользователей
void MainWindow::slotUpdateUserList(QStandardItemModel *model) {

    // установка модели в таблицу
    ui->tableView_users->setModel(NULL);
    ui->tableView_users->setModel(model);

    // сокрытие ненужных столбцов
    for (int i = 0; i < 16; i++) {
        if (ui->tableView_users
                ->model()
                ->headerData(i, Qt::Horizontal)
                .toString() == "empty")
            ui->tableView_users->setColumnHidden(i, true);

    // растягивающиеся столбцы
    ui->tableView_users->setColumnWidth(0, 300);
    for (int i = 1; i < model->columnCount(); i++)
        ui->tableView_users
            ->horizontalHeader()
            ->setSectionResizeMode(i, QHeaderView::Stretch);

    // сортировка
    ui->tableView_users->selectColumn(1);
    ui->tableView_users->sortByColumn(1, Qt::AscendingOrder);
    ui->tableView_users->setSortingEnabled(true);

    // настройка верхушки таблицы
    ui->tableView_users->setStyleSheet(
                            "QHeaderView::section "
                            "{ background-color:lightGray }"
                        );
    }

    // разблокировка основного окна приложения
    ui->statusbar->showMessage("Подключено");
    ui->centralwidget->setEnabled(true);
}

// отправка на сервер настроек доступа
void MainWindow::slotEditAccess(quint8 attr, const QString &date) {

    // выбранные пользователи
    QModelIndexList indexes = ui->tableView_users
                                  ->selectionModel()
                                  ->selection().indexes();

    // цикл по выделенным ячейкам таблицы
    QStringList users;
    users.append(date);
    for (int i = 0; i < indexes.count(); ++i) {

        // составление списка имен пользователей на
        if (indexes.at(i).column() == 0)
            users.append(indexes.at(i)
                             .siblingAtColumn(0)
                             .data()
                             .toString());
    }
    mTCPhandler->sendEditAccessReq(attr, users);
}

// отправка запроса на отключение доступа
void MainWindow::slotDelAccess(quint8 attr) {

    // выбранные пользователи
    QModelIndexList indexes = ui->tableView_users
                                  ->selectionModel()
                                  ->selection().indexes();

    // цикл по выделенным ячейкам таблицы
    QStringList users;
    for (int i = 0; i < indexes.count(); ++i) {

        // составление списка имен пользователей на сервер
        if (indexes.at(i).column() == 0)
            users.append(indexes.at(i)
                             .siblingAtColumn(0)
                             .data()
                             .toString());
    }

    // отправка списка пользователей на сервер
    mTCPhandler->sendDelAccessReq(attr, users);
}

// завершение операции с доступом или паролем
void MainWindow::slotOperationReady(bool resetPWD) {
    ui->centralwidget->setEnabled(true);
    mProgressDialog->close();
    mProgressDialog->slotUpdateStatus(100, 100, "", "Complete");
    if (!resetPWD)
        mTCPhandler->sendUpdStructReq(ui->lineEdit_search->text());
}

// ошибки соединения
void MainWindow::slotConnectError(const QString &errorMsg) {
    mAccessDialog->close();                                             // закрытие диалогового окна доступа
    mProgressDialog->close();                                           // закрытие диалогового окна прогресса
    mProgressDialog->slotUpdateStatus(100, 100, "error", "error");      // закрытие диалогового окна прогресса - 2
    ui->tableView_users->setModel(NULL);                                // обнуление таблицы
    ui->lineEdit_search->clear();                                       // очистка поля поиска
    ui->statusbar->showMessage("Отключено");                            // статус приложения "Отключено"
    ui->centralwidget->setEnabled(false);                               // блокировка основного окна приложения
    QMessageBox::critical(this, "Ошибка", errorMsg);                    // сообщение об ошибке
    mLoginDialog->show();                                               // вывод диалогового окна авторизации
}

// кнопка поиска
void MainWindow::on_pushButton_search_clicked() {
    mTCPhandler->sendUpdStructReq(ui->lineEdit_search->text());
}

// кнопка установка доступа
void MainWindow::on_pushButton_access_clicked() {
    slotClickButton(true);
}

// кнопка сброса пароля
void MainWindow::on_pushButton_reset_clicked() {
    slotClickButton(false);
}
