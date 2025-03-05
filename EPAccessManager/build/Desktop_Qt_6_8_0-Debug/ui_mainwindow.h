/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_search;
    QLineEdit *lineEdit_search;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_access;
    QPushButton *pushButton_reset;
    QTableView *tableView_users;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        pushButton_search = new QPushButton(centralwidget);
        pushButton_search->setObjectName("pushButton_search");

        horizontalLayout->addWidget(pushButton_search);

        lineEdit_search = new QLineEdit(centralwidget);
        lineEdit_search->setObjectName("lineEdit_search");

        horizontalLayout->addWidget(lineEdit_search);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_access = new QPushButton(centralwidget);
        pushButton_access->setObjectName("pushButton_access");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton_access->sizePolicy().hasHeightForWidth());
        pushButton_access->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(pushButton_access);

        pushButton_reset = new QPushButton(centralwidget);
        pushButton_reset->setObjectName("pushButton_reset");
        sizePolicy.setHeightForWidth(pushButton_reset->sizePolicy().hasHeightForWidth());
        pushButton_reset->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(pushButton_reset);


        verticalLayout->addLayout(horizontalLayout);

        tableView_users = new QTableView(centralwidget);
        tableView_users->setObjectName("tableView_users");
        tableView_users->setMouseTracking(true);
        tableView_users->setTabletTracking(true);
        tableView_users->setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
        tableView_users->setDragDropOverwriteMode(true);
        tableView_users->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
        tableView_users->verticalHeader()->setVisible(false);

        verticalLayout->addWidget(tableView_users);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "EPAccessManager", nullptr));
        pushButton_search->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\320\270\321\201\320\272", nullptr));
        pushButton_access->setText(QCoreApplication::translate("MainWindow", "\320\243\320\277\321\200\320\260\320\262\320\273\320\265\320\275\320\270\320\265 \320\264\320\276\321\201\321\202\321\203\320\277\320\276\320\274", nullptr));
        pushButton_reset->setText(QCoreApplication::translate("MainWindow", "\320\241\320\261\321\200\320\276\321\201 \320\277\320\260\321\200\320\276\320\273\321\217", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
