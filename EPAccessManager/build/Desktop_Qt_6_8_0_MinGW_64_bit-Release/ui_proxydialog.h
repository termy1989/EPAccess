/********************************************************************************
** Form generated from reading UI file 'proxydialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROXYDIALOG_H
#define UI_PROXYDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ProxyDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QLabel *label_url;
    QLineEdit *lineEdit_url;
    QLabel *label_port;
    QSpinBox *spinBox_port;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QLabel *label_login;
    QLineEdit *lineEdit_login;
    QLabel *label_password;
    QLineEdit *lineEdit_password;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QRadioButton *radioButton_http;
    QRadioButton *radioButton_httpcaching;
    QRadioButton *radioButton_socks5;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *checkBox_proxy_off;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ProxyDialog)
    {
        if (ProxyDialog->objectName().isEmpty())
            ProxyDialog->setObjectName("ProxyDialog");
        ProxyDialog->resize(370, 199);
        verticalLayout_2 = new QVBoxLayout(ProxyDialog);
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label_url = new QLabel(ProxyDialog);
        label_url->setObjectName("label_url");

        horizontalLayout->addWidget(label_url);

        lineEdit_url = new QLineEdit(ProxyDialog);
        lineEdit_url->setObjectName("lineEdit_url");

        horizontalLayout->addWidget(lineEdit_url);

        label_port = new QLabel(ProxyDialog);
        label_port->setObjectName("label_port");

        horizontalLayout->addWidget(label_port);

        spinBox_port = new QSpinBox(ProxyDialog);
        spinBox_port->setObjectName("spinBox_port");
        spinBox_port->setMinimum(0);
        spinBox_port->setMaximum(65534);

        horizontalLayout->addWidget(spinBox_port);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        groupBox_2 = new QGroupBox(ProxyDialog);
        groupBox_2->setObjectName("groupBox_2");
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setObjectName("gridLayout");
        label_login = new QLabel(groupBox_2);
        label_login->setObjectName("label_login");

        gridLayout->addWidget(label_login, 0, 0, 1, 1);

        lineEdit_login = new QLineEdit(groupBox_2);
        lineEdit_login->setObjectName("lineEdit_login");

        gridLayout->addWidget(lineEdit_login, 0, 1, 1, 1);

        label_password = new QLabel(groupBox_2);
        label_password->setObjectName("label_password");

        gridLayout->addWidget(label_password, 1, 0, 1, 1);

        lineEdit_password = new QLineEdit(groupBox_2);
        lineEdit_password->setObjectName("lineEdit_password");
        lineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);

        gridLayout->addWidget(lineEdit_password, 1, 1, 1, 1);


        horizontalLayout_2->addWidget(groupBox_2);

        groupBox = new QGroupBox(ProxyDialog);
        groupBox->setObjectName("groupBox");
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName("verticalLayout");
        radioButton_http = new QRadioButton(groupBox);
        radioButton_http->setObjectName("radioButton_http");
        radioButton_http->setChecked(true);

        verticalLayout->addWidget(radioButton_http);

        radioButton_httpcaching = new QRadioButton(groupBox);
        radioButton_httpcaching->setObjectName("radioButton_httpcaching");

        verticalLayout->addWidget(radioButton_httpcaching);

        radioButton_socks5 = new QRadioButton(groupBox);
        radioButton_socks5->setObjectName("radioButton_socks5");

        verticalLayout->addWidget(radioButton_socks5);


        horizontalLayout_2->addWidget(groupBox);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        checkBox_proxy_off = new QCheckBox(ProxyDialog);
        checkBox_proxy_off->setObjectName("checkBox_proxy_off");

        horizontalLayout_3->addWidget(checkBox_proxy_off);

        buttonBox = new QDialogButtonBox(ProxyDialog);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        horizontalLayout_3->addWidget(buttonBox);


        verticalLayout_2->addLayout(horizontalLayout_3);


        retranslateUi(ProxyDialog);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, ProxyDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, ProxyDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(ProxyDialog);
    } // setupUi

    void retranslateUi(QDialog *ProxyDialog)
    {
        ProxyDialog->setWindowTitle(QCoreApplication::translate("ProxyDialog", "\320\235\320\260\321\201\321\202\321\200\320\276\320\271\320\272\320\260 \320\277\321\200\320\276\320\272\321\201\320\270", nullptr));
        label_url->setText(QCoreApplication::translate("ProxyDialog", "\320\220\320\264\321\200\320\265\321\201:", nullptr));
        label_port->setText(QCoreApplication::translate("ProxyDialog", "\320\237\320\276\321\200\321\202:", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("ProxyDialog", "\320\220\320\262\321\202\320\276\321\200\320\270\320\267\320\260\321\206\320\270\321\217", nullptr));
        label_login->setText(QCoreApplication::translate("ProxyDialog", "\320\233\320\276\320\263\320\270\320\275:", nullptr));
        label_password->setText(QCoreApplication::translate("ProxyDialog", "\320\237\320\260\321\200\320\276\320\273\321\214:", nullptr));
        groupBox->setTitle(QCoreApplication::translate("ProxyDialog", "\320\242\320\270\320\277 \320\277\321\200\320\276\320\272\321\201\320\270", nullptr));
        radioButton_http->setText(QCoreApplication::translate("ProxyDialog", "Http", nullptr));
        radioButton_httpcaching->setText(QCoreApplication::translate("ProxyDialog", "HttpCaching", nullptr));
        radioButton_socks5->setText(QCoreApplication::translate("ProxyDialog", "Socks5", nullptr));
        checkBox_proxy_off->setText(QCoreApplication::translate("ProxyDialog", "\320\221\320\265\320\267 \320\277\321\200\320\276\320\272\321\201\320\270", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ProxyDialog: public Ui_ProxyDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROXYDIALOG_H
