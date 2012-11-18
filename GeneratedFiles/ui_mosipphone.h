/********************************************************************************
** Form generated from reading UI file 'mosipphone.ui'
**
** Created: Mon Sep 19 01:51:19 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MOSIPPHONE_H
#define UI_MOSIPPHONE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MosipPhone
{
public:
    QVBoxLayout *verticalLayout_4;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QToolButton *toolButton_4;
    QComboBox *comboBox_7;
    QComboBox *comboBox_4;
    QToolButton *toolButton_2;
    QWidget *widget_2;
    QGridLayout *gridLayout;
    QToolButton *toolButton_15;
    QToolButton *toolButton_16;
    QToolButton *toolButton_17;
    QSpacerItem *horizontalSpacer_3;
    QToolButton *toolButton_18;
    QToolButton *toolButton_19;
    QToolButton *toolButton_20;
    QToolButton *toolButton_21;
    QToolButton *toolButton_22;
    QToolButton *toolButton_23;
    QSpacerItem *horizontalSpacer_2;
    QToolButton *toolButton_24;
    QToolButton *toolButton_14;
    QToolButton *toolButton_25;
    QComboBox *comboBox;
    QSpacerItem *verticalSpacer;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QWidget *page_2;
    QHBoxLayout *horizontalLayout_3;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_2;
    QLabel *label_4;
    QPushButton *pushButton_8;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_6;
    QComboBox *comboBox_9;
    QComboBox *comboBox_10;
    QToolButton *toolButton_9;
    QTreeView *treeView_2;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_5;
    QPushButton *pushButton_7;
    QListWidget *listWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QToolButton *toolButton;

    void setupUi(QWidget *MosipPhone)
    {
        if (MosipPhone->objectName().isEmpty())
            MosipPhone->setObjectName(QString::fromUtf8("MosipPhone"));
        MosipPhone->resize(374, 329);
        verticalLayout_4 = new QVBoxLayout(MosipPhone);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        tabWidget = new QTabWidget(MosipPhone);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        toolButton_4 = new QToolButton(tab);
        toolButton_4->setObjectName(QString::fromUtf8("toolButton_4"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/skins/default/keyboard.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_4->setIcon(icon);
        toolButton_4->setCheckable(true);
        toolButton_4->setChecked(true);

        horizontalLayout_4->addWidget(toolButton_4);

        comboBox_7 = new QComboBox(tab);
        comboBox_7->setObjectName(QString::fromUtf8("comboBox_7"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(comboBox_7->sizePolicy().hasHeightForWidth());
        comboBox_7->setSizePolicy(sizePolicy1);
        comboBox_7->setMinimumSize(QSize(0, 40));
        QFont font;
        font.setPointSize(13);
        font.setBold(false);
        font.setWeight(50);
        font.setKerning(true);
        comboBox_7->setFont(font);
        comboBox_7->setEditable(true);

        horizontalLayout_4->addWidget(comboBox_7);

        comboBox_4 = new QComboBox(tab);
        comboBox_4->setObjectName(QString::fromUtf8("comboBox_4"));
        sizePolicy1.setHeightForWidth(comboBox_4->sizePolicy().hasHeightForWidth());
        comboBox_4->setSizePolicy(sizePolicy1);
        comboBox_4->setMaximumSize(QSize(105, 16777215));

        horizontalLayout_4->addWidget(comboBox_4);

        toolButton_2 = new QToolButton(tab);
        toolButton_2->setObjectName(QString::fromUtf8("toolButton_2"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/skins/default/progress.gif"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_2->setIcon(icon1);

        horizontalLayout_4->addWidget(toolButton_2);


        verticalLayout_2->addLayout(horizontalLayout_4);

        widget_2 = new QWidget(tab);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        gridLayout = new QGridLayout(widget_2);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        toolButton_15 = new QToolButton(widget_2);
        toolButton_15->setObjectName(QString::fromUtf8("toolButton_15"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/skins/default/digit1.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_15->setIcon(icon2);
        toolButton_15->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_15, 0, 1, 1, 1);

        toolButton_16 = new QToolButton(widget_2);
        toolButton_16->setObjectName(QString::fromUtf8("toolButton_16"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/skins/default/digit2.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_16->setIcon(icon3);
        toolButton_16->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_16, 0, 2, 1, 1);

        toolButton_17 = new QToolButton(widget_2);
        toolButton_17->setObjectName(QString::fromUtf8("toolButton_17"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/skins/default/digit3.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_17->setIcon(icon4);
        toolButton_17->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_17, 0, 3, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(25, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 1, 0, 1, 1);

        toolButton_18 = new QToolButton(widget_2);
        toolButton_18->setObjectName(QString::fromUtf8("toolButton_18"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/skins/default/digit4.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_18->setIcon(icon5);
        toolButton_18->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_18, 1, 1, 1, 1);

        toolButton_19 = new QToolButton(widget_2);
        toolButton_19->setObjectName(QString::fromUtf8("toolButton_19"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/skins/default/digit5.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_19->setIcon(icon6);
        toolButton_19->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_19, 1, 2, 1, 1);

        toolButton_20 = new QToolButton(widget_2);
        toolButton_20->setObjectName(QString::fromUtf8("toolButton_20"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/skins/default/digit6.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_20->setIcon(icon7);
        toolButton_20->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_20, 1, 3, 1, 1);

        toolButton_21 = new QToolButton(widget_2);
        toolButton_21->setObjectName(QString::fromUtf8("toolButton_21"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/skins/default/digit7.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_21->setIcon(icon8);
        toolButton_21->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_21, 2, 1, 1, 1);

        toolButton_22 = new QToolButton(widget_2);
        toolButton_22->setObjectName(QString::fromUtf8("toolButton_22"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/skins/default/digit8.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_22->setIcon(icon9);
        toolButton_22->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_22, 2, 2, 1, 1);

        toolButton_23 = new QToolButton(widget_2);
        toolButton_23->setObjectName(QString::fromUtf8("toolButton_23"));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/skins/default/digit9.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_23->setIcon(icon10);
        toolButton_23->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_23, 2, 3, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(75, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 2, 4, 1, 1);

        toolButton_24 = new QToolButton(widget_2);
        toolButton_24->setObjectName(QString::fromUtf8("toolButton_24"));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/skins/default/digitstar.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_24->setIcon(icon11);
        toolButton_24->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_24, 3, 1, 1, 1);

        toolButton_14 = new QToolButton(widget_2);
        toolButton_14->setObjectName(QString::fromUtf8("toolButton_14"));
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/skins/default/digit0.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_14->setIcon(icon12);
        toolButton_14->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_14, 3, 2, 1, 1);

        toolButton_25 = new QToolButton(widget_2);
        toolButton_25->setObjectName(QString::fromUtf8("toolButton_25"));
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/skins/default/digitpound.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_25->setIcon(icon13);
        toolButton_25->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_25, 3, 3, 1, 1);

        comboBox = new QComboBox(widget_2);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        gridLayout->addWidget(comboBox, 1, 4, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 4, 2, 1, 1);


        verticalLayout_2->addWidget(widget_2);

        stackedWidget = new QStackedWidget(tab);
        stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
        page = new QWidget();
        page->setObjectName(QString::fromUtf8("page"));
        horizontalLayout_2 = new QHBoxLayout(page);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pushButton_4 = new QPushButton(page);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setMinimumSize(QSize(0, 60));
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/skins/default/images.jpeg"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_4->setIcon(icon14);
        pushButton_4->setIconSize(QSize(32, 32));

        horizontalLayout_2->addWidget(pushButton_4);

        pushButton_5 = new QPushButton(page);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        pushButton_5->setEnabled(false);
        pushButton_5->setMinimumSize(QSize(0, 60));
        pushButton_5->setSizeIncrement(QSize(0, 0));
        pushButton_5->setBaseSize(QSize(0, 0));
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/skins/default/phone_32.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_5->setIcon(icon15);
        pushButton_5->setIconSize(QSize(32, 32));

        horizontalLayout_2->addWidget(pushButton_5);

        pushButton_6 = new QPushButton(page);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        pushButton_6->setMinimumSize(QSize(0, 60));
        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/skins/default/clear.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_6->setIcon(icon16);
        pushButton_6->setIconSize(QSize(32, 32));

        horizontalLayout_2->addWidget(pushButton_6);

        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName(QString::fromUtf8("page_2"));
        horizontalLayout_3 = new QHBoxLayout(page_2);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label = new QLabel(page_2);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy2);

        gridLayout_2->addWidget(label, 0, 0, 1, 1);

        label_3 = new QLabel(page_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout_2->addWidget(label_3, 0, 1, 1, 1);

        label_2 = new QLabel(page_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy2.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy2);

        gridLayout_2->addWidget(label_2, 1, 0, 1, 1);

        label_4 = new QLabel(page_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        gridLayout_2->addWidget(label_4, 1, 1, 1, 1);


        horizontalLayout_3->addLayout(gridLayout_2);

        pushButton_8 = new QPushButton(page_2);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        pushButton_8->setEnabled(false);
        QSizePolicy sizePolicy3(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(pushButton_8->sizePolicy().hasHeightForWidth());
        pushButton_8->setSizePolicy(sizePolicy3);
        pushButton_8->setMinimumSize(QSize(0, 60));
        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/skins/default/hangup_20.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_8->setIcon(icon17);

        horizontalLayout_3->addWidget(pushButton_8);

        stackedWidget->addWidget(page_2);

        verticalLayout_2->addWidget(stackedWidget);

        QIcon icon18;
        icon18.addFile(QString::fromUtf8(":/skins/default/phone.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tab, icon18, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayout_3 = new QVBoxLayout(tab_2);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        comboBox_9 = new QComboBox(tab_2);
        comboBox_9->setObjectName(QString::fromUtf8("comboBox_9"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(comboBox_9->sizePolicy().hasHeightForWidth());
        comboBox_9->setSizePolicy(sizePolicy4);
        comboBox_9->setEditable(true);

        horizontalLayout_6->addWidget(comboBox_9);

        comboBox_10 = new QComboBox(tab_2);
        comboBox_10->setObjectName(QString::fromUtf8("comboBox_10"));
        sizePolicy1.setHeightForWidth(comboBox_10->sizePolicy().hasHeightForWidth());
        comboBox_10->setSizePolicy(sizePolicy1);

        horizontalLayout_6->addWidget(comboBox_10);

        toolButton_9 = new QToolButton(tab_2);
        toolButton_9->setObjectName(QString::fromUtf8("toolButton_9"));

        horizontalLayout_6->addWidget(toolButton_9);


        verticalLayout_3->addLayout(horizontalLayout_6);

        treeView_2 = new QTreeView(tab_2);
        treeView_2->setObjectName(QString::fromUtf8("treeView_2"));
        treeView_2->setContextMenuPolicy(Qt::CustomContextMenu);
        treeView_2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        treeView_2->setAlternatingRowColors(true);
        treeView_2->setAnimated(true);

        verticalLayout_3->addWidget(treeView_2);

        QIcon icon19;
        icon19.addFile(QString::fromUtf8(":/skins/default/images_call_log.jpeg"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tab_2, icon19, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        verticalLayout_5 = new QVBoxLayout(tab_3);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        pushButton_7 = new QPushButton(tab_3);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));

        verticalLayout_5->addWidget(pushButton_7);

        listWidget = new QListWidget(tab_3);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        verticalLayout_5->addWidget(listWidget);

        QIcon icon20;
        icon20.addFile(QString::fromUtf8(":/skins/default/sendchat_32.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tab_3, icon20, QString());

        verticalLayout_4->addWidget(tabWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton = new QPushButton(MosipPhone);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        QIcon icon21;
        icon21.addFile(QString::fromUtf8(":/skins/default/book_32.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton->setIcon(icon21);

        horizontalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(MosipPhone);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        QIcon icon22;
        icon22.addFile(QString::fromUtf8(":/skins/default/configure.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_2->setIcon(icon22);

        horizontalLayout->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(MosipPhone);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        QIcon icon23;
        icon23.addFile(QString::fromUtf8(":/skins/default/quest.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_3->setIcon(icon23);

        horizontalLayout->addWidget(pushButton_3);

        toolButton = new QToolButton(MosipPhone);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));
        toolButton->setArrowType(Qt::LeftArrow);

        horizontalLayout->addWidget(toolButton);


        verticalLayout_4->addLayout(horizontalLayout);


        retranslateUi(MosipPhone);

        tabWidget->setCurrentIndex(0);
        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MosipPhone);
    } // setupUi

    void retranslateUi(QWidget *MosipPhone)
    {
        MosipPhone->setWindowTitle(QApplication::translate("MosipPhone", "Form", 0, QApplication::UnicodeUTF8));
        toolButton_4->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
        comboBox_7->clear();
        comboBox_7->insertItems(0, QStringList()
         << QApplication::translate("MosipPhone", "*013800138000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MosipPhone", "*01068730066", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MosipPhone", "*01096196", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        comboBox_7->setToolTip(QApplication::translate("MosipPhone", "phone number: such as 123-12344556", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        toolButton_2->setText(QString());
        toolButton_15->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_16->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_17->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_18->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_19->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_20->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_21->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_22->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_23->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_24->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_14->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_25->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        comboBox->setToolTip(QApplication::translate("MosipPhone", "\350\257\255\351\237\263\347\274\226\347\240\201", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton_4->setText(QApplication::translate("MosipPhone", "QuickNum", 0, QApplication::UnicodeUTF8));
        pushButton_5->setText(QApplication::translate("MosipPhone", "Call ...", 0, QApplication::UnicodeUTF8));
        pushButton_6->setText(QApplication::translate("MosipPhone", "Clear", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MosipPhone", "Time:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MosipPhone", "00:00:00", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MosipPhone", "State:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MosipPhone", "CALLING", 0, QApplication::UnicodeUTF8));
        pushButton_8->setText(QApplication::translate("MosipPhone", "Hangup", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MosipPhone", "Dial", 0, QApplication::UnicodeUTF8));
        comboBox_10->clear();
        comboBox_10->insertItems(0, QStringList()
         << QApplication::translate("MosipPhone", "All", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MosipPhone", "Missed", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MosipPhone", "Received", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MosipPhone", "Deleted", 0, QApplication::UnicodeUTF8)
        );
        toolButton_9->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MosipPhone", "Call Histories", 0, QApplication::UnicodeUTF8));
        pushButton_7->setText(QApplication::translate("MosipPhone", "PushButton", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("MosipPhone", "Messages", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("MosipPhone", "Accounts", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("MosipPhone", "Settings", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("MosipPhone", "Help", 0, QApplication::UnicodeUTF8));
        toolButton->setText(QApplication::translate("MosipPhone", "...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MosipPhone: public Ui_MosipPhone {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MOSIPPHONE_H
