/********************************************************************************
** Form generated from reading UI file 'progressdialog.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROGRESSDIALOG_H
#define UI_PROGRESSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ProgressDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_user;
    QProgressBar *progressBar;
    QLabel *label_action;

    void setupUi(QDialog *ProgressDialog)
    {
        if (ProgressDialog->objectName().isEmpty())
            ProgressDialog->setObjectName("ProgressDialog");
        ProgressDialog->resize(400, 170);
        verticalLayout = new QVBoxLayout(ProgressDialog);
        verticalLayout->setObjectName("verticalLayout");
        label_user = new QLabel(ProgressDialog);
        label_user->setObjectName("label_user");

        verticalLayout->addWidget(label_user);

        progressBar = new QProgressBar(ProgressDialog);
        progressBar->setObjectName("progressBar");
        progressBar->setValue(24);

        verticalLayout->addWidget(progressBar);

        label_action = new QLabel(ProgressDialog);
        label_action->setObjectName("label_action");

        verticalLayout->addWidget(label_action);


        retranslateUi(ProgressDialog);

        QMetaObject::connectSlotsByName(ProgressDialog);
    } // setupUi

    void retranslateUi(QDialog *ProgressDialog)
    {
        ProgressDialog->setWindowTitle(QCoreApplication::translate("ProgressDialog", "\320\222\321\213\320\277\320\276\320\273\320\275\320\265\320\275\320\270\320\265...", nullptr));
        label_user->setText(QCoreApplication::translate("ProgressDialog", "TextLabel", nullptr));
        label_action->setText(QCoreApplication::translate("ProgressDialog", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ProgressDialog: public Ui_ProgressDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROGRESSDIALOG_H
