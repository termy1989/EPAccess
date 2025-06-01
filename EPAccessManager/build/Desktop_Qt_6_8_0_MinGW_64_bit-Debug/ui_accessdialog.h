/********************************************************************************
** Form generated from reading UI file 'accessdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ACCESSDIALOG_H
#define UI_ACCESSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_AccessDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_type;
    QComboBox *comboBox_access;
    QHBoxLayout *horizontalLayout;
    QLabel *label_date;
    QDateEdit *dateEdit_access;
    QCheckBox *checkBox_vip;
    QDialogButtonBox *buttonBox_main;

    void setupUi(QDialog *AccessDialog)
    {
        if (AccessDialog->objectName().isEmpty())
            AccessDialog->setObjectName("AccessDialog");
        AccessDialog->resize(235, 130);
        verticalLayout_3 = new QVBoxLayout(AccessDialog);
        verticalLayout_3->setObjectName("verticalLayout_3");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label_type = new QLabel(AccessDialog);
        label_type->setObjectName("label_type");

        horizontalLayout_2->addWidget(label_type);

        comboBox_access = new QComboBox(AccessDialog);
        comboBox_access->setObjectName("comboBox_access");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(comboBox_access->sizePolicy().hasHeightForWidth());
        comboBox_access->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(comboBox_access);


        verticalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label_date = new QLabel(AccessDialog);
        label_date->setObjectName("label_date");

        horizontalLayout->addWidget(label_date);

        dateEdit_access = new QDateEdit(AccessDialog);
        dateEdit_access->setObjectName("dateEdit_access");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(dateEdit_access->sizePolicy().hasHeightForWidth());
        dateEdit_access->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(dateEdit_access);


        verticalLayout_3->addLayout(horizontalLayout);

        checkBox_vip = new QCheckBox(AccessDialog);
        checkBox_vip->setObjectName("checkBox_vip");

        verticalLayout_3->addWidget(checkBox_vip);

        buttonBox_main = new QDialogButtonBox(AccessDialog);
        buttonBox_main->setObjectName("buttonBox_main");
        buttonBox_main->setOrientation(Qt::Orientation::Horizontal);
        buttonBox_main->setStandardButtons(QDialogButtonBox::StandardButton::Cancel|QDialogButtonBox::StandardButton::Ok);

        verticalLayout_3->addWidget(buttonBox_main);


        retranslateUi(AccessDialog);
        QObject::connect(buttonBox_main, &QDialogButtonBox::accepted, AccessDialog, qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox_main, &QDialogButtonBox::rejected, AccessDialog, qOverload<>(&QDialog::reject));

        QMetaObject::connectSlotsByName(AccessDialog);
    } // setupUi

    void retranslateUi(QDialog *AccessDialog)
    {
        AccessDialog->setWindowTitle(QCoreApplication::translate("AccessDialog", "\320\243\321\201\321\202\320\260\320\275\320\276\320\262\320\272\320\260 \320\264\320\276\321\201\321\202\321\203\320\277\320\260", nullptr));
        label_type->setText(QCoreApplication::translate("AccessDialog", "\320\242\320\270\320\277 \320\264\320\276\321\201\321\202\321\203\320\277\320\260:", nullptr));
        label_date->setText(QCoreApplication::translate("AccessDialog", "\320\237\320\276\321\201\320\273\320\265\320\264\320\275\320\270\320\271 \320\264\320\265\320\275\321\214  \320\264\320\276\321\201\321\202\321\203\320\277\320\260:", nullptr));
        dateEdit_access->setDisplayFormat(QCoreApplication::translate("AccessDialog", "dd.MM.yy", nullptr));
        checkBox_vip->setText(QCoreApplication::translate("AccessDialog", "\320\237\321\200\320\265\320\264\320\276\321\201\321\202\320\260\320\262\320\270\321\202\321\214 \320\277\320\276\321\201\321\202\320\276\321\217\320\275\320\275\321\213\320\271 \320\264\320\276\321\201\321\202\321\203\320\277", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AccessDialog: public Ui_AccessDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ACCESSDIALOG_H
