/********************************************************************************
** Form generated from reading UI file 'preferencesdialog.ui'
**
** Created: Mon Sep 19 01:51:19 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PREFERENCESDIALOG_H
#define UI_PREFERENCESDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QListWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PreferencesDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QListWidget *listWidget;
    QStackedWidget *stackedWidget;
    QWidget *page_4;
    QLabel *label_9;
    QWidget *page_3;
    QLabel *label_10;
    QWidget *page;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_6;
    QGridLayout *gridLayout;
    QLabel *label;
    QCheckBox *checkBox;
    QLabel *label_2;
    QComboBox *comboBox_4;
    QLabel *label_3;
    QComboBox *comboBox_5;
    QLabel *label_4;
    QComboBox *comboBox_6;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_5;
    QLineEdit *lineEdit;
    QLabel *label_8;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_7;
    QCheckBox *checkBox_2;
    QSpacerItem *verticalSpacer;
    QWidget *page_2;
    QListView *listView;
    QLabel *label_11;
    QWidget *page_5;
    QLabel *label_13;
    QWidget *page_6;
    QLabel *label_12;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *PreferencesDialog)
    {
        if (PreferencesDialog->objectName().isEmpty())
            PreferencesDialog->setObjectName(QString::fromUtf8("PreferencesDialog"));
        PreferencesDialog->resize(583, 355);
        verticalLayout = new QVBoxLayout(PreferencesDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        listWidget = new QListWidget(PreferencesDialog);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(listWidget->sizePolicy().hasHeightForWidth());
        listWidget->setSizePolicy(sizePolicy);
        listWidget->setMinimumSize(QSize(0, 0));
        listWidget->setMaximumSize(QSize(180, 16777215));
        QFont font;
        font.setPointSize(10);
        font.setBold(false);
        font.setWeight(50);
        listWidget->setFont(font);

        horizontalLayout->addWidget(listWidget);

        stackedWidget = new QStackedWidget(PreferencesDialog);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(stackedWidget->sizePolicy().hasHeightForWidth());
        stackedWidget->setSizePolicy(sizePolicy1);
        page_4 = new QWidget();
        page_4->setObjectName(QString::fromUtf8("page_4"));
        label_9 = new QLabel(page_4);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(20, 10, 57, 15));
        stackedWidget->addWidget(page_4);
        page_3 = new QWidget();
        page_3->setObjectName(QString::fromUtf8("page_3"));
        label_10 = new QLabel(page_3);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(20, 20, 57, 15));
        stackedWidget->addWidget(page_3);
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        verticalLayout_2 = new QVBoxLayout(page);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_6 = new QLabel(page);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        QFont font1;
        font1.setBold(true);
        font1.setUnderline(true);
        font1.setWeight(75);
        label_6->setFont(font1);

        verticalLayout_2->addWidget(label_6);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(page);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        checkBox = new QCheckBox(page);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        gridLayout->addWidget(checkBox, 0, 1, 1, 1);

        label_2 = new QLabel(page);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        comboBox_4 = new QComboBox(page);
        comboBox_4->setObjectName(QString::fromUtf8("comboBox_4"));

        gridLayout->addWidget(comboBox_4, 1, 1, 1, 1);

        label_3 = new QLabel(page);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        comboBox_5 = new QComboBox(page);
        comboBox_5->setObjectName(QString::fromUtf8("comboBox_5"));

        gridLayout->addWidget(comboBox_5, 2, 1, 1, 1);

        label_4 = new QLabel(page);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        comboBox_6 = new QComboBox(page);
        comboBox_6->setObjectName(QString::fromUtf8("comboBox_6"));

        gridLayout->addWidget(comboBox_6, 3, 1, 1, 1);


        verticalLayout_2->addLayout(gridLayout);

        widget = new QWidget(page);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setEnabled(true);
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_5 = new QLabel(widget);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        horizontalLayout_2->addWidget(label_5);

        lineEdit = new QLineEdit(widget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout_2->addWidget(lineEdit);


        verticalLayout_2->addWidget(widget);

        label_8 = new QLabel(page);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setFont(font1);

        verticalLayout_2->addWidget(label_8);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_7 = new QLabel(page);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_3->addWidget(label_7);

        checkBox_2 = new QCheckBox(page);
        checkBox_2->setObjectName(QString::fromUtf8("checkBox_2"));

        horizontalLayout_3->addWidget(checkBox_2);


        verticalLayout_2->addLayout(horizontalLayout_3);

        verticalSpacer = new QSpacerItem(20, 79, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        listView = new QListView(page_2);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setGeometry(QRect(40, 100, 256, 192));
        label_11 = new QLabel(page_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(30, 30, 57, 15));
        stackedWidget->addWidget(page_2);
        page_5 = new QWidget();
        page_5->setObjectName(QString::fromUtf8("page_5"));
        label_13 = new QLabel(page_5);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(70, 80, 57, 15));
        stackedWidget->addWidget(page_5);
        page_6 = new QWidget();
        page_6->setObjectName(QString::fromUtf8("page_6"));
        label_12 = new QLabel(page_6);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(50, 50, 57, 15));
        stackedWidget->addWidget(page_6);

        horizontalLayout->addWidget(stackedWidget);


        verticalLayout->addLayout(horizontalLayout);

        buttonBox = new QDialogButtonBox(PreferencesDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Help|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(PreferencesDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), PreferencesDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), PreferencesDialog, SLOT(reject()));

        stackedWidget->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(PreferencesDialog);
    } // setupUi

    void retranslateUi(QDialog *PreferencesDialog)
    {
        PreferencesDialog->setWindowTitle(QApplication::translate("PreferencesDialog", "Preferences Dialog", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = listWidget->isSortingEnabled();
        listWidget->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = listWidget->item(0);
        ___qlistwidgetitem->setText(QApplication::translate("PreferencesDialog", "General", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem1 = listWidget->item(1);
        ___qlistwidgetitem1->setText(QApplication::translate("PreferencesDialog", "Network", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem2 = listWidget->item(2);
        ___qlistwidgetitem2->setText(QApplication::translate("PreferencesDialog", "SIP", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem3 = listWidget->item(3);
        ___qlistwidgetitem3->setText(QApplication::translate("PreferencesDialog", "Skype", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem4 = listWidget->item(4);
        ___qlistwidgetitem4->setText(QApplication::translate("PreferencesDialog", "VOEP", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem5 = listWidget->item(5);
        ___qlistwidgetitem5->setText(QApplication::translate("PreferencesDialog", "Advance", 0, QApplication::UnicodeUTF8));
        listWidget->setSortingEnabled(__sortingEnabled);

        label_9->setText(QApplication::translate("PreferencesDialog", "Genral", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("PreferencesDialog", "Network", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("PreferencesDialog", "SIP options", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("PreferencesDialog", "Use TLS:", 0, QApplication::UnicodeUTF8));
        checkBox->setText(QString());
        label_2->setText(QApplication::translate("PreferencesDialog", "SIP Protocol:", 0, QApplication::UnicodeUTF8));
        comboBox_4->clear();
        comboBox_4->insertItems(0, QStringList()
         << QApplication::translate("PreferencesDialog", "UDP", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PreferencesDialog", "TCP", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PreferencesDialog", "TLS", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PreferencesDialog", "IPV6", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PreferencesDialog", "UDP6", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PreferencesDialog", "TCP6", 0, QApplication::UnicodeUTF8)
        );
        label_3->setText(QApplication::translate("PreferencesDialog", "Codecs:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        comboBox_5->setToolTip(QApplication::translate("PreferencesDialog", "\350\257\255\351\237\263\347\274\226\347\240\201", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_4->setText(QApplication::translate("PreferencesDialog", "NAT Detect:", 0, QApplication::UnicodeUTF8));
        comboBox_6->clear();
        comboBox_6->insertItems(0, QStringList()
         << QApplication::translate("PreferencesDialog", "NONE", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PreferencesDialog", "STUN", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PreferencesDialog", "TURN", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PreferencesDialog", "ICE", 0, QApplication::UnicodeUTF8)
        );
        label_5->setText(QApplication::translate("PreferencesDialog", "    STUN Server:", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("PreferencesDialog", "\n"
"Advanced", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("PreferencesDialog", "Skype Bridge Mode:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        checkBox_2->setToolTip(QApplication::translate("PreferencesDialog", "Using your skype as network transport layer\n"
"This method supply raliable transport when call initialized, \n"
"but may be little problem when call initializing ,\n"
"because we must check your skype installation, \n"
"run state , login state and app to skype connect status.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        checkBox_2->setText(QString());
        label_11->setText(QApplication::translate("PreferencesDialog", "Skype", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("PreferencesDialog", "VOEP", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("PreferencesDialog", "Advanced", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PreferencesDialog: public Ui_PreferencesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PREFERENCESDIALOG_H
