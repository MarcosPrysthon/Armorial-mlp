/********************************************************************************
** Form generated from reading UI file 'mlpmainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MLPMAINWINDOW_H
#define UI_MLPMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MlpMainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *kickButton;
    QPushButton *passButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MlpMainWindow)
    {
        if (MlpMainWindow->objectName().isEmpty())
            MlpMainWindow->setObjectName(QStringLiteral("MlpMainWindow"));
        MlpMainWindow->resize(240, 270);
        centralwidget = new QWidget(MlpMainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        kickButton = new QPushButton(centralwidget);
        kickButton->setObjectName(QStringLiteral("kickButton"));
        kickButton->setGeometry(QRect(70, 130, 89, 25));
        passButton = new QPushButton(centralwidget);
        passButton->setObjectName(QStringLiteral("passButton"));
        passButton->setGeometry(QRect(70, 80, 89, 25));
        MlpMainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MlpMainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 240, 22));
        MlpMainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MlpMainWindow);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MlpMainWindow->setStatusBar(statusbar);

        retranslateUi(MlpMainWindow);

        QMetaObject::connectSlotsByName(MlpMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MlpMainWindow)
    {
        MlpMainWindow->setWindowTitle(QApplication::translate("MlpMainWindow", "MainWindow", Q_NULLPTR));
        kickButton->setText(QApplication::translate("MlpMainWindow", "Chute", Q_NULLPTR));
        passButton->setText(QApplication::translate("MlpMainWindow", "Passe", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MlpMainWindow: public Ui_MlpMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MLPMAINWINDOW_H
