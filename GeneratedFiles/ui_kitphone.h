/********************************************************************************
** Form generated from reading UI file 'kitphone.ui'
**
** Created: Mon Sep 19 01:51:18 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KITPHONE_H
#define UI_KITPHONE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_KitPhone
{
public:
    QAction *actionDsfdsfdsf;
    QAction *action;
    QAction *action_Exit;
    QAction *action_About;
    QAction *actionSip_Phone_Mode;
    QAction *actionSkype_Phone_Mode;
    QAction *actionEmpty_Phone_Mode;
    QAction *action_2;
    QAction *actionPreferences;
    QAction *actionAccounts;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_Help;
    QMenu *menu_2;

    void setupUi(QMainWindow *KitPhone)
    {
        if (KitPhone->objectName().isEmpty())
            KitPhone->setObjectName(QString::fromUtf8("KitPhone"));
        KitPhone->resize(391, 90);
        KitPhone->setMouseTracking(false);
        actionDsfdsfdsf = new QAction(KitPhone);
        actionDsfdsfdsf->setObjectName(QString::fromUtf8("actionDsfdsfdsf"));
        action = new QAction(KitPhone);
        action->setObjectName(QString::fromUtf8("action"));
        action_Exit = new QAction(KitPhone);
        action_Exit->setObjectName(QString::fromUtf8("action_Exit"));
        action_About = new QAction(KitPhone);
        action_About->setObjectName(QString::fromUtf8("action_About"));
        actionSip_Phone_Mode = new QAction(KitPhone);
        actionSip_Phone_Mode->setObjectName(QString::fromUtf8("actionSip_Phone_Mode"));
        actionSip_Phone_Mode->setCheckable(true);
        actionSip_Phone_Mode->setAutoRepeat(false);
        actionSkype_Phone_Mode = new QAction(KitPhone);
        actionSkype_Phone_Mode->setObjectName(QString::fromUtf8("actionSkype_Phone_Mode"));
        actionSkype_Phone_Mode->setCheckable(true);
        actionSkype_Phone_Mode->setAutoRepeat(false);
        actionEmpty_Phone_Mode = new QAction(KitPhone);
        actionEmpty_Phone_Mode->setObjectName(QString::fromUtf8("actionEmpty_Phone_Mode"));
        actionEmpty_Phone_Mode->setCheckable(true);
        actionEmpty_Phone_Mode->setAutoRepeat(false);
        action_2 = new QAction(KitPhone);
        action_2->setObjectName(QString::fromUtf8("action_2"));
        actionPreferences = new QAction(KitPhone);
        actionPreferences->setObjectName(QString::fromUtf8("actionPreferences"));
        actionAccounts = new QAction(KitPhone);
        actionAccounts->setObjectName(QString::fromUtf8("actionAccounts"));
        centralWidget = new QWidget(KitPhone);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(-1, 1, -1, -1);
        KitPhone->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(KitPhone);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        KitPhone->setStatusBar(statusBar);
        menuBar = new QMenuBar(KitPhone);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 391, 20));
        menu = new QMenu(menuBar);
        menu->setObjectName(QString::fromUtf8("menu"));
        menu_Help = new QMenu(menuBar);
        menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QString::fromUtf8("menu_2"));
        KitPhone->setMenuBar(menuBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menuBar->addAction(menu_Help->menuAction());
        menu->addAction(actionAccounts);
        menu->addAction(actionPreferences);
        menu->addSeparator();
        menu->addAction(action_Exit);
        menu_Help->addAction(action_About);
        menu_2->addAction(action_2);

        retranslateUi(KitPhone);

        QMetaObject::connectSlotsByName(KitPhone);
    } // setupUi

    void retranslateUi(QMainWindow *KitPhone)
    {
        KitPhone->setWindowTitle(QApplication::translate("KitPhone", "Kitphone SIP/Skype Soft  phone", 0, QApplication::UnicodeUTF8));
        actionDsfdsfdsf->setText(QApplication::translate("KitPhone", "dsfdsfdsf", 0, QApplication::UnicodeUTF8));
        action->setText(QApplication::translate("KitPhone", "Sip Account Settings", 0, QApplication::UnicodeUTF8));
        action_Exit->setText(QApplication::translate("KitPhone", "&Exit", 0, QApplication::UnicodeUTF8));
        action_About->setText(QApplication::translate("KitPhone", "&About Kitphone", 0, QApplication::UnicodeUTF8));
        actionSip_Phone_Mode->setText(QApplication::translate("KitPhone", "Sip Phone Mode", 0, QApplication::UnicodeUTF8));
        actionSkype_Phone_Mode->setText(QApplication::translate("KitPhone", "Skype Phone Mode", 0, QApplication::UnicodeUTF8));
        actionEmpty_Phone_Mode->setText(QApplication::translate("KitPhone", "Empty Phone Mode", 0, QApplication::UnicodeUTF8));
        action_2->setText(QApplication::translate("KitPhone", "\347\275\221\347\273\234\346\243\200\346\265\213", 0, QApplication::UnicodeUTF8));
        actionPreferences->setText(QApplication::translate("KitPhone", "Preferences", 0, QApplication::UnicodeUTF8));
        actionAccounts->setText(QApplication::translate("KitPhone", "Accounts", 0, QApplication::UnicodeUTF8));
        menu->setTitle(QApplication::translate("KitPhone", "\346\226\207\344\273\266(&F)", 0, QApplication::UnicodeUTF8));
        menu_Help->setTitle(QApplication::translate("KitPhone", "\345\270\256\345\212\251(&H)", 0, QApplication::UnicodeUTF8));
        menu_2->setTitle(QApplication::translate("KitPhone", "\345\267\245\345\205\267(&T)", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class KitPhone: public Ui_KitPhone {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KITPHONE_H
