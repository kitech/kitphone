/********************************************************************************
** Form generated from reading UI file 'skypephone.ui'
**
** Created: Mon Sep 19 01:51:19 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SKYPEPHONE_H
#define UI_SKYPEPHONE_H

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
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SkypePhone
{
public:
    QVBoxLayout *verticalLayout_6;
    QLabel *label;
    QGridLayout *gridLayout_2;
    QComboBox *comboBox_3;
    QToolButton *toolButton_2;
    QPushButton *pushButton_4;
    QPushButton *pushButton_3;
    QLabel *label_14;
    QLabel *label_3;
    QLabel *label_15;
    QWidget *widget_2;
    QGridLayout *gridLayout;
    QToolButton *toolButton_15;
    QToolButton *toolButton_16;
    QToolButton *toolButton_17;
    QPushButton *pushButton;
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
    QPushButton *pushButton_2;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_10;
    QLabel *label_2;
    QLabel *label_11;
    QSpacerItem *horizontalSpacer_5;
    QToolButton *toolButton;
    QLabel *label_12;
    QHBoxLayout *horizontalLayout_8;
    QSlider *verticalSlider;
    QLabel *label_7;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_5;
    QLabel *label_6;
    QSlider *verticalSlider_2;
    QHBoxLayout *horizontalLayout_7;
    QToolButton *toolButton_12;
    QSpacerItem *horizontalSpacer_4;
    QToolButton *toolButton_10;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_5;
    QToolButton *toolButton_3;
    QComboBox *comboBox_8;
    QToolButton *toolButton_7;
    QToolButton *toolButton_8;
    QTreeView *treeView;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_6;
    QComboBox *comboBox_9;
    QComboBox *comboBox_10;
    QToolButton *toolButton_9;
    QTreeView *treeView_2;
    QListWidget *listWidget;

    void setupUi(QWidget *SkypePhone)
    {
        if (SkypePhone->objectName().isEmpty())
            SkypePhone->setObjectName(QString::fromUtf8("SkypePhone"));
        SkypePhone->resize(299, 614);
        SkypePhone->setContextMenuPolicy(Qt::DefaultContextMenu);
        verticalLayout_6 = new QVBoxLayout(SkypePhone);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        label = new QLabel(SkypePhone);
        label->setObjectName(QString::fromUtf8("label"));
        label->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        verticalLayout_6->addWidget(label);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        comboBox_3 = new QComboBox(SkypePhone);
        comboBox_3->setObjectName(QString::fromUtf8("comboBox_3"));
        QFont font;
        font.setPointSize(13);
        comboBox_3->setFont(font);
        comboBox_3->setEditable(true);

        gridLayout_2->addWidget(comboBox_3, 1, 1, 1, 2);

        toolButton_2 = new QToolButton(SkypePhone);
        toolButton_2->setObjectName(QString::fromUtf8("toolButton_2"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/skins/default/keyboard.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_2->setIcon(icon);
        toolButton_2->setCheckable(true);
        toolButton_2->setChecked(true);
        toolButton_2->setAutoRaise(false);

        gridLayout_2->addWidget(toolButton_2, 1, 0, 1, 1);

        pushButton_4 = new QPushButton(SkypePhone);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setEnabled(false);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/skins/default/phone.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_4->setIcon(icon1);

        gridLayout_2->addWidget(pushButton_4, 1, 3, 1, 1);

        pushButton_3 = new QPushButton(SkypePhone);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton_3->sizePolicy().hasHeightForWidth());
        pushButton_3->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(pushButton_3, 0, 3, 1, 1);

        label_14 = new QLabel(SkypePhone);
        label_14->setObjectName(QString::fromUtf8("label_14"));
        label_14->setPixmap(QPixmap(QString::fromUtf8(":/skins/default/status_offline.png")));
        label_14->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_14, 0, 0, 1, 1);

        label_3 = new QLabel(SkypePhone);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy1);
        label_3->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_2->addWidget(label_3, 0, 1, 1, 1);

        label_15 = new QLabel(SkypePhone);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        gridLayout_2->addWidget(label_15, 0, 2, 1, 1);


        verticalLayout_6->addLayout(gridLayout_2);

        widget_2 = new QWidget(SkypePhone);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        gridLayout = new QGridLayout(widget_2);
        gridLayout->setContentsMargins(1, 1, 1, 1);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setHorizontalSpacing(1);
        gridLayout->setVerticalSpacing(0);
        toolButton_15 = new QToolButton(widget_2);
        toolButton_15->setObjectName(QString::fromUtf8("toolButton_15"));
        toolButton_15->setEnabled(true);
        toolButton_15->setMaximumSize(QSize(40, 26));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/skins/default/digit1.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon2.addFile(QString::fromUtf8(":/skins/default/digit1_hover.png"), QSize(), QIcon::Active, QIcon::On);
        toolButton_15->setIcon(icon2);
        toolButton_15->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_15, 0, 1, 1, 1);

        toolButton_16 = new QToolButton(widget_2);
        toolButton_16->setObjectName(QString::fromUtf8("toolButton_16"));
        toolButton_16->setMaximumSize(QSize(40, 26));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/skins/default/digit2.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_16->setIcon(icon3);
        toolButton_16->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_16, 0, 2, 1, 1);

        toolButton_17 = new QToolButton(widget_2);
        toolButton_17->setObjectName(QString::fromUtf8("toolButton_17"));
        toolButton_17->setMaximumSize(QSize(40, 26));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/skins/default/digit3.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_17->setIcon(icon4);
        toolButton_17->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_17, 0, 3, 1, 1);

        pushButton = new QPushButton(widget_2);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        gridLayout->addWidget(pushButton, 0, 4, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(80, 25, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 1, 0, 1, 1);

        toolButton_18 = new QToolButton(widget_2);
        toolButton_18->setObjectName(QString::fromUtf8("toolButton_18"));
        toolButton_18->setMaximumSize(QSize(40, 26));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/skins/default/digit4.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_18->setIcon(icon5);
        toolButton_18->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_18, 1, 1, 1, 1);

        toolButton_19 = new QToolButton(widget_2);
        toolButton_19->setObjectName(QString::fromUtf8("toolButton_19"));
        toolButton_19->setMaximumSize(QSize(40, 26));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/skins/default/digit5.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_19->setIcon(icon6);
        toolButton_19->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_19, 1, 2, 1, 1);

        toolButton_20 = new QToolButton(widget_2);
        toolButton_20->setObjectName(QString::fromUtf8("toolButton_20"));
        toolButton_20->setMaximumSize(QSize(40, 26));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/skins/default/digit6.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_20->setIcon(icon7);
        toolButton_20->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_20, 1, 3, 1, 1);

        toolButton_21 = new QToolButton(widget_2);
        toolButton_21->setObjectName(QString::fromUtf8("toolButton_21"));
        toolButton_21->setMaximumSize(QSize(40, 26));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/skins/default/digit7.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_21->setIcon(icon8);
        toolButton_21->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_21, 2, 1, 1, 1);

        toolButton_22 = new QToolButton(widget_2);
        toolButton_22->setObjectName(QString::fromUtf8("toolButton_22"));
        toolButton_22->setMaximumSize(QSize(40, 26));
        toolButton_22->setStyleSheet(QString::fromUtf8(""));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/skins/default/digit8.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_22->setIcon(icon9);
        toolButton_22->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_22, 2, 2, 1, 1);

        toolButton_23 = new QToolButton(widget_2);
        toolButton_23->setObjectName(QString::fromUtf8("toolButton_23"));
        toolButton_23->setMaximumSize(QSize(40, 26));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/skins/default/digit9.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_23->setIcon(icon10);
        toolButton_23->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_23, 2, 3, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(79, 25, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 2, 4, 1, 1);

        toolButton_24 = new QToolButton(widget_2);
        toolButton_24->setObjectName(QString::fromUtf8("toolButton_24"));
        toolButton_24->setMaximumSize(QSize(40, 26));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/skins/default/digitstar.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_24->setIcon(icon11);
        toolButton_24->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_24, 3, 1, 1, 1);

        toolButton_14 = new QToolButton(widget_2);
        toolButton_14->setObjectName(QString::fromUtf8("toolButton_14"));
        toolButton_14->setMaximumSize(QSize(40, 26));
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/skins/default/digit0.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_14->setIcon(icon12);
        toolButton_14->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_14, 3, 2, 1, 1);

        toolButton_25 = new QToolButton(widget_2);
        toolButton_25->setObjectName(QString::fromUtf8("toolButton_25"));
        toolButton_25->setMaximumSize(QSize(40, 26));
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/skins/default/digitpound.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_25->setIcon(icon13);
        toolButton_25->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_25, 3, 3, 1, 1);

        pushButton_2 = new QPushButton(widget_2);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        gridLayout->addWidget(pushButton_2, 3, 4, 1, 1);


        verticalLayout_6->addWidget(widget_2);

        widget = new QWidget(SkypePhone);
        widget->setObjectName(QString::fromUtf8("widget"));
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        label_10 = new QLabel(widget);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setPixmap(QPixmap(QString::fromUtf8(":/skins/default/speaker.png")));

        horizontalLayout_9->addWidget(label_10);

        label_2 = new QLabel(widget);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_9->addWidget(label_2);

        label_11 = new QLabel(widget);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setCursor(QCursor(Qt::PointingHandCursor));
        label_11->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        horizontalLayout_9->addWidget(label_11);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_5);

        toolButton = new QToolButton(widget);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));
        toolButton->setCheckable(true);
        toolButton->setChecked(true);
        toolButton->setAutoRaise(false);
        toolButton->setArrowType(Qt::DownArrow);

        horizontalLayout_9->addWidget(toolButton);

        label_12 = new QLabel(widget);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setPixmap(QPixmap(QString::fromUtf8(":/skins/default/Un-mute_Alpha_down_16x16_s1.png")));
        label_12->setScaledContents(false);

        horizontalLayout_9->addWidget(label_12);


        verticalLayout->addLayout(horizontalLayout_9);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        verticalSlider = new QSlider(widget);
        verticalSlider->setObjectName(QString::fromUtf8("verticalSlider"));
        verticalSlider->setMinimumSize(QSize(0, 40));
        verticalSlider->setMaximumSize(QSize(16777215, 50));
        verticalSlider->setSingleStep(5);
        verticalSlider->setValue(99);
        verticalSlider->setOrientation(Qt::Vertical);
        verticalSlider->setTickPosition(QSlider::TicksBothSides);

        horizontalLayout_8->addWidget(verticalSlider);

        label_7 = new QLabel(widget);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setMaximumSize(QSize(35, 16777215));
        label_7->setPixmap(QPixmap(QString::fromUtf8(":/skins/default/user.png")));
        label_7->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(label_7);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label_5 = new QLabel(widget);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        verticalLayout_4->addWidget(label_5);

        label_6 = new QLabel(widget);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        verticalLayout_4->addWidget(label_6);


        horizontalLayout_8->addLayout(verticalLayout_4);

        verticalSlider_2 = new QSlider(widget);
        verticalSlider_2->setObjectName(QString::fromUtf8("verticalSlider_2"));
        verticalSlider_2->setMinimumSize(QSize(0, 40));
        verticalSlider_2->setMaximumSize(QSize(16777215, 50));
        verticalSlider_2->setSingleStep(5);
        verticalSlider_2->setValue(99);
        verticalSlider_2->setOrientation(Qt::Vertical);
        verticalSlider_2->setTickPosition(QSlider::TicksBothSides);

        horizontalLayout_8->addWidget(verticalSlider_2);


        verticalLayout->addLayout(horizontalLayout_8);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        toolButton_12 = new QToolButton(widget);
        toolButton_12->setObjectName(QString::fromUtf8("toolButton_12"));
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/skins/default/Call_video_start_GreenStart_32x32_s1.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_12->setIcon(icon14);
        toolButton_12->setIconSize(QSize(20, 20));

        horizontalLayout_7->addWidget(toolButton_12);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_4);

        toolButton_10 = new QToolButton(widget);
        toolButton_10->setObjectName(QString::fromUtf8("toolButton_10"));
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/skins/default/hangup_20.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_10->setIcon(icon15);
        toolButton_10->setIconSize(QSize(20, 20));

        horizontalLayout_7->addWidget(toolButton_10);


        verticalLayout->addLayout(horizontalLayout_7);


        verticalLayout_6->addWidget(widget);

        tabWidget = new QTabWidget(SkypePhone);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setDocumentMode(true);
        tabWidget->setMovable(true);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        toolButton_3 = new QToolButton(tab);
        toolButton_3->setObjectName(QString::fromUtf8("toolButton_3"));
        toolButton_3->setContextMenuPolicy(Qt::CustomContextMenu);
        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/skins/default/addcontact.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_3->setIcon(icon16);
        toolButton_3->setAutoRepeat(false);
        toolButton_3->setAutoExclusive(true);
        toolButton_3->setPopupMode(QToolButton::MenuButtonPopup);
        toolButton_3->setToolButtonStyle(Qt::ToolButtonIconOnly);
        toolButton_3->setAutoRaise(false);
        toolButton_3->setArrowType(Qt::NoArrow);

        horizontalLayout_5->addWidget(toolButton_3);

        comboBox_8 = new QComboBox(tab);
        comboBox_8->setObjectName(QString::fromUtf8("comboBox_8"));
        comboBox_8->setEditable(true);

        horizontalLayout_5->addWidget(comboBox_8);

        toolButton_7 = new QToolButton(tab);
        toolButton_7->setObjectName(QString::fromUtf8("toolButton_7"));
        toolButton_7->setArrowType(Qt::DownArrow);

        horizontalLayout_5->addWidget(toolButton_7);

        toolButton_8 = new QToolButton(tab);
        toolButton_8->setObjectName(QString::fromUtf8("toolButton_8"));

        horizontalLayout_5->addWidget(toolButton_8);


        verticalLayout_2->addLayout(horizontalLayout_5);

        treeView = new QTreeView(tab);
        treeView->setObjectName(QString::fromUtf8("treeView"));
        treeView->setContextMenuPolicy(Qt::CustomContextMenu);
        treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        treeView->setAlternatingRowColors(true);

        verticalLayout_2->addWidget(treeView);

        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/skins/default/book_32.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tab, icon17, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayout_3 = new QVBoxLayout(tab_2);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        comboBox_9 = new QComboBox(tab_2);
        comboBox_9->setObjectName(QString::fromUtf8("comboBox_9"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(comboBox_9->sizePolicy().hasHeightForWidth());
        comboBox_9->setSizePolicy(sizePolicy2);
        comboBox_9->setEditable(true);

        horizontalLayout_6->addWidget(comboBox_9);

        comboBox_10 = new QComboBox(tab_2);
        comboBox_10->setObjectName(QString::fromUtf8("comboBox_10"));

        horizontalLayout_6->addWidget(comboBox_10);

        toolButton_9 = new QToolButton(tab_2);
        toolButton_9->setObjectName(QString::fromUtf8("toolButton_9"));

        horizontalLayout_6->addWidget(toolButton_9);


        verticalLayout_3->addLayout(horizontalLayout_6);

        treeView_2 = new QTreeView(tab_2);
        treeView_2->setObjectName(QString::fromUtf8("treeView_2"));
        treeView_2->setContextMenuPolicy(Qt::CustomContextMenu);
        treeView_2->setAlternatingRowColors(true);

        verticalLayout_3->addWidget(treeView_2);

        tabWidget->addTab(tab_2, QString());

        verticalLayout_6->addWidget(tabWidget);

        listWidget = new QListWidget(SkypePhone);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setAlternatingRowColors(true);

        verticalLayout_6->addWidget(listWidget);


        retranslateUi(SkypePhone);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(SkypePhone);
    } // setupUi

    void retranslateUi(QWidget *SkypePhone)
    {
        SkypePhone->setWindowTitle(QApplication::translate("SkypePhone", "Kitphone for Skype", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SkypePhone", "\344\275\277\347\224\250TOM-Skype\350\256\241\346\227\266\351\200\232\346\213\250\346\211\223\347\224\265\350\257\235\357\274\232Skype\351\232\247\351\201\223\346\250\241\345\274\217", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        comboBox_3->setToolTip(QApplication::translate("SkypePhone", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'\303\246\302\226\302\207\303\246\302\263\302\211\303\251\302\251\302\277\303\246\302\255\302\243\303\251\302\273\302\221'; font-size:13pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">\346\240\274\345\274\217\357\274\232 *010678881xx \346\210\226 *0138001380xx</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        toolButton_2->setToolTip(QApplication::translate("SkypePhone", "\346\230\276\347\244\272/\351\232\220\350\227\217\346\213\250\345\217\267\347\233\230", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        toolButton_2->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButton_4->setToolTip(QApplication::translate("SkypePhone", "\345\274\200\345\247\213\345\221\274\345\217\253\347\224\265\350\257\235", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton_4->setText(QApplication::translate("SkypePhone", "\345\221\274\345\217\253", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButton_3->setToolTip(QApplication::translate("SkypePhone", "\344\275\277\347\224\250Skype\345\205\254\345\205\261API\350\277\236\346\216\245\345\210\260Skype\345\256\242\346\210\267\347\253\257", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton_3->setText(QApplication::translate("SkypePhone", "\350\277\236\346\216\245Skype", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        label_14->setToolTip(QApplication::translate("SkypePhone", "Skype\345\256\242\346\210\267\347\253\257\347\212\266\346\200\201: NONE", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_14->setText(QString());
#ifndef QT_NO_TOOLTIP
        label_3->setToolTip(QApplication::translate("SkypePhone", "\345\275\223\345\211\215\347\231\273\351\231\206\347\232\204Skype\347\224\250\346\210\267\345\220\215", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_3->setText(QApplication::translate("SkypePhone", "\346\234\252\350\277\236\346\216\245Skype", 0, QApplication::UnicodeUTF8));
        label_15->setText(QString());
        toolButton_15->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_16->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_17->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("SkypePhone", "API\350\257\246\346\203\205", 0, QApplication::UnicodeUTF8));
        toolButton_18->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_19->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_20->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_21->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_22->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_23->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_24->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_14->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_25->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("SkypePhone", "\346\227\245\345\277\227\347\252\227\345\217\243", 0, QApplication::UnicodeUTF8));
        label_10->setText(QString());
        label_2->setText(QString());
        label_11->setText(QApplication::translate("SkypePhone", "call status message here", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        toolButton->setToolTip(QApplication::translate("SkypePhone", "\346\230\276\347\244\272\345\256\214\346\225\264\346\227\245\345\277\227\350\256\260\345\275\225\343\200\202", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        toolButton->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        label_12->setText(QString());
#ifndef QT_NO_TOOLTIP
        verticalSlider->setToolTip(QApplication::translate("SkypePhone", "Mix Volume", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_7->setText(QString());
        label_5->setText(QApplication::translate("SkypePhone", "*89010234556", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        label_6->setToolTip(QApplication::translate("SkypePhone", "\350\242\253\345\221\274\345\217\253\347\224\250\346\210\267\346\211\200\345\234\250\345\245\275\345\217\213\345\210\206\347\273\204", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_6->setText(QApplication::translate("SkypePhone", "\346\211\200\345\234\250\347\273\204\357\274\232Others", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        verticalSlider_2->setToolTip(QApplication::translate("SkypePhone", "Mic Volume", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        toolButton_12->setToolTip(QApplication::translate("SkypePhone", "\345\274\200\345\220\257\350\247\206\351\242\221", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        toolButton_12->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        toolButton_10->setToolTip(QApplication::translate("SkypePhone", "\346\214\202\346\226\255\351\200\232\350\257\235", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        toolButton_10->setText(QApplication::translate("SkypePhone", "\346\214\202\346\226\255", 0, QApplication::UnicodeUTF8));
        toolButton_3->setText(QString());
        toolButton_7->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        toolButton_8->setToolTip(QApplication::translate("SkypePhone", "\346\265\256\350\265\267\347\252\227\345\217\243", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        toolButton_8->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("SkypePhone", "\350\201\224\347\263\273\344\272\272", 0, QApplication::UnicodeUTF8));
        comboBox_10->clear();
        comboBox_10->insertItems(0, QStringList()
         << QApplication::translate("SkypePhone", "All", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SkypePhone", "Missed", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SkypePhone", "Recieved", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SkypePhone", "Deleted", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        toolButton_9->setToolTip(QApplication::translate("SkypePhone", "\346\265\256\350\265\267\347\252\227\345\217\243", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        toolButton_9->setText(QApplication::translate("SkypePhone", "...", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("SkypePhone", "\345\221\274\345\217\253\345\216\206\345\217\262", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SkypePhone: public Ui_SkypePhone {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SKYPEPHONE_H
