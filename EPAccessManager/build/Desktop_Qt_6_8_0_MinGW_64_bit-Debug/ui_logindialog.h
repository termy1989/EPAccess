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
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_password;
    QLabel *label_login;
    QLineEdit *lineEdit_password;
    QLineEdit *lineEdit_login;
    QLineEdit *lineEdit_url;
    QLabel *label_url;
    QCheckBox *checkBox_save;
    QDialogButtonBox *buttonBox;
    QLabel *label_advanced;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName("LoginDialog");
        LoginDialog->resize(326, 157);
        layoutWidget = new QWidget(LoginDialog);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(10, 10, 304, 138));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_password = new QLabel(layoutWidget);
        label_password->setObjectName("label_password");
        label_password->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_password, 2, 0, 1, 1);

        label_login = new QLabel(layoutWidget);
        label_login->setObjectName("label_login");
        label_login->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_login, 1, 0, 1, 1);

        lineEdit_password = new QLineEdit(layoutWidget);
        lineEdit_password->setObjectName("lineEdit_password");
        lineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);

        gridLayout->addWidget(lineEdit_password, 2, 1, 1, 1);

        lineEdit_login = new QLineEdit(layoutWidget);
        lineEdit_login->setObjectName("lineEdit_login");

        gridLayout->addWidget(lineEdit_login, 1, 1, 1, 1);

        lineEdit_url = new QLineEdit(layoutWidget);
        lineEdit_url->setObjectName("lineEdit_url");

        gridLayout->addWidget(lineEdit_url, 0, 1, 1, 1);

        label_url = new QLabel(layoutWidget);
        label_url->setObjectName("label_url");
        label_url->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        label_url->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        gridLayout->addWidget(label_url, 0, 0, 1, 1);

        checkBox_save = new QCheckBox(layoutWidget);
        checkBox_save->setObjectName("checkBox_save");

        gridLayout->addWidget(checkBox_save, 3, 1, 1, 1);

        buttonBox = new QDialogButtonBox(layoutWidget);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        gridLayout->addWidget(buttonBox, 4, 1, 1, 1);

        label_advanced = new QLabel(layoutWidget);
        label_advanced->setObjectName("label_advanced");

        gridLayout->addWidget(label_advanced, 3, 0, 1, 1);


        retranslateUi(LoginDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, LoginDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, LoginDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QCoreApplication::translate("LoginDialog", "\320\220\320\262\321\202\320\276\321\200\320\270\320\267\320\260\321\206\320\270\321\217", nullptr));
        label_password->setText(QCoreApplication::translate("LoginDialog", "\320\237\320\260\321\200\320\276\320\273\321\214 ", nullptr));
        label_login->setText(QCoreApplication::translate("LoginDialog", "\320\233\320\276\320\263\320\270\320\275 ", nullptr));
        label_url->setText(QCoreApplication::translate("LoginDialog", "URL:Port ", nullptr));
        checkBox_save->setText(QCoreApplication::translate("LoginDialog", "\320\227\320\260\320\277\320\276\320\274\320\275\320\270\321\202\321\214 \320\275\320\260 \321\215\321\202\320\276\320\274 \320\272\320\276\320\274\320\277\321\214\321\216\321\202\320\265\321\200\320\265", nullptr));
        label_advanced->setText(QCoreApplication::translate("LoginDialog", "\320\224\320\276\320\277\320\276\320\273\320\275\320\270\321\202\320\265\320\273\321\214\320\275\320\276: ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
