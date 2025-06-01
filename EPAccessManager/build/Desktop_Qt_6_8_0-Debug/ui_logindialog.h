/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QGridLayout *gridLayout;
    QDialogButtonBox *buttonBox;
    QLineEdit *lineEdit_url;
    QLineEdit *lineEdit_login;
    QPushButton *proxyButton;
    QLineEdit *lineEdit_password;
    QCheckBox *checkBox_save;
    QLabel *label_url;
    QLabel *label_login;
    QLabel *label_password;
    QLabel *label_advanced;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName("LoginDialog");
        LoginDialog->resize(336, 155);
        gridLayout = new QGridLayout(LoginDialog);
        gridLayout->setObjectName("gridLayout");
        buttonBox = new QDialogButtonBox(LoginDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        gridLayout->addWidget(buttonBox, 5, 1, 1, 1);

        lineEdit_url = new QLineEdit(LoginDialog);
        lineEdit_url->setObjectName("lineEdit_url");

        gridLayout->addWidget(lineEdit_url, 1, 1, 1, 1);

        lineEdit_login = new QLineEdit(LoginDialog);
        lineEdit_login->setObjectName("lineEdit_login");

        gridLayout->addWidget(lineEdit_login, 2, 1, 1, 1);

        proxyButton = new QPushButton(LoginDialog);
        proxyButton->setObjectName("proxyButton");

        gridLayout->addWidget(proxyButton, 5, 0, 1, 1);

        lineEdit_password = new QLineEdit(LoginDialog);
        lineEdit_password->setObjectName("lineEdit_password");
        lineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);

        gridLayout->addWidget(lineEdit_password, 3, 1, 1, 1);

        checkBox_save = new QCheckBox(LoginDialog);
        checkBox_save->setObjectName("checkBox_save");

        gridLayout->addWidget(checkBox_save, 4, 1, 1, 1);

        label_url = new QLabel(LoginDialog);
        label_url->setObjectName("label_url");
        label_url->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_url, 1, 0, 1, 1);

        label_login = new QLabel(LoginDialog);
        label_login->setObjectName("label_login");
        label_login->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_login, 2, 0, 1, 1);

        label_password = new QLabel(LoginDialog);
        label_password->setObjectName("label_password");
        label_password->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_password, 3, 0, 1, 1);

        label_advanced = new QLabel(LoginDialog);
        label_advanced->setObjectName("label_advanced");
        label_advanced->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_advanced, 4, 0, 1, 1);


        retranslateUi(LoginDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, LoginDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, LoginDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "\320\220\320\262\321\202\320\276\321\200\320\270\320\267\320\260\321\206\320\270\321\217", nullptr));
        proxyButton->setText(QCoreApplication::translate("LoginDialog", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\260 \320\277\321\200\320\276\320\272\321\201\320\270", nullptr));
        checkBox_save->setText(QCoreApplication::translate("LoginDialog", "\320\227\320\260\320\277\320\276\320\274\320\275\320\270\321\202\321\214 \320\275\320\260 \321\215\321\202\320\276\320\274 \320\272\320\276\320\274\320\277\321\214\321\216\321\202\320\265\321\200\320\265", nullptr));
        label_url->setText(QCoreApplication::translate("LoginDialog", "URL:Port", nullptr));
        label_login->setText(QCoreApplication::translate("LoginDialog", "\320\233\320\276\320\263\320\270\320\275", nullptr));
        label_password->setText(QCoreApplication::translate("LoginDialog", "\320\237\320\260\321\200\320\276\320\273\321\214", nullptr));
        label_advanced->setText(QCoreApplication::translate("LoginDialog", "\320\224\320\276\320\277\320\276\320\273\320\275\320\270\321\202\320\265\320\273\321\214\320\275\320\276", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
