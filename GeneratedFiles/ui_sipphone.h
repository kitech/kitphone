/********************************************************************************
** Form generated from reading UI file 'sipphone.ui'
**
** Created: Mon Sep 19 01:51:19 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIPPHONE_H
#define UI_SIPPHONE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
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

class Ui_SipPhone
{
public:
    QVBoxLayout *verticalLayout_5;
    QLabel *label_13;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QComboBox *comboBox_5;
    QComboBox *comboBox_6;
    QToolButton *toolButton_2;
    QToolButton *toolButton_3;
    QHBoxLayout *horizontalLayout_4;
    QToolButton *toolButton_4;
    QComboBox *comboBox_7;
    QPushButton *pushButton_11;
    QWidget *widget;
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
    QComboBox *comboBox_2;
    QComboBox *comboBox;
    QComboBox *comboBox_3;
    QCheckBox *checkBox;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_10;
    QLabel *label;
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
    QToolButton *toolButton_5;
    QToolButton *toolButton_12;
    QSpacerItem *horizontalSpacer_4;
    QToolButton *toolButton_10;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_5;
    QToolButton *toolButton_6;
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

    void setupUi(QWidget *SipPhone)
    {
        if (SipPhone->objectName().isEmpty())
            SipPhone->setObjectName(QString::fromUtf8("SipPhone"));
        SipPhone->resize(322, 639);
        SipPhone->setMouseTracking(false);
        verticalLayout_5 = new QVBoxLayout(SipPhone);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        label_13 = new QLabel(SipPhone);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_13->sizePolicy().hasHeightForWidth());
        label_13->setSizePolicy(sizePolicy);
        label_13->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        verticalLayout_5->addWidget(label_13);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton_5 = new QPushButton(SipPhone);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        sizePolicy.setHeightForWidth(pushButton_5->sizePolicy().hasHeightForWidth());
        pushButton_5->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(pushButton_5);

        pushButton_6 = new QPushButton(SipPhone);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        sizePolicy.setHeightForWidth(pushButton_6->sizePolicy().hasHeightForWidth());
        pushButton_6->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(pushButton_6);


        verticalLayout_5->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_4 = new QLabel(SipPhone);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setPixmap(QPixmap(QString::fromUtf8(":/skins/default/muc.png")));

        horizontalLayout_3->addWidget(label_4);

        comboBox_5 = new QComboBox(SipPhone);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/skins/default/status_online.png"), QSize(), QIcon::Normal, QIcon::Off);
        comboBox_5->addItem(icon, QString());
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/skins/default/status_offline.png"), QSize(), QIcon::Normal, QIcon::Off);
        comboBox_5->addItem(icon1, QString());
        comboBox_5->setObjectName(QString::fromUtf8("comboBox_5"));
        sizePolicy.setHeightForWidth(comboBox_5->sizePolicy().hasHeightForWidth());
        comboBox_5->setSizePolicy(sizePolicy);

        horizontalLayout_3->addWidget(comboBox_5);

        comboBox_6 = new QComboBox(SipPhone);
        comboBox_6->setObjectName(QString::fromUtf8("comboBox_6"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(comboBox_6->sizePolicy().hasHeightForWidth());
        comboBox_6->setSizePolicy(sizePolicy1);

        horizontalLayout_3->addWidget(comboBox_6);

        toolButton_2 = new QToolButton(SipPhone);
        toolButton_2->setObjectName(QString::fromUtf8("toolButton_2"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/skins/default/speaker.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_2->setIcon(icon2);

        horizontalLayout_3->addWidget(toolButton_2);

        toolButton_3 = new QToolButton(SipPhone);
        toolButton_3->setObjectName(QString::fromUtf8("toolButton_3"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/skins/default/Call_video_start_GreenStart_32x32_s1.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_3->setIcon(icon3);

        horizontalLayout_3->addWidget(toolButton_3);


        verticalLayout_5->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        toolButton_4 = new QToolButton(SipPhone);
        toolButton_4->setObjectName(QString::fromUtf8("toolButton_4"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/skins/default/keyboard.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_4->setIcon(icon4);
        toolButton_4->setCheckable(true);
        toolButton_4->setChecked(true);

        horizontalLayout_4->addWidget(toolButton_4);

        comboBox_7 = new QComboBox(SipPhone);
        comboBox_7->setObjectName(QString::fromUtf8("comboBox_7"));
        sizePolicy.setHeightForWidth(comboBox_7->sizePolicy().hasHeightForWidth());
        comboBox_7->setSizePolicy(sizePolicy);
        comboBox_7->setMinimumSize(QSize(0, 32));
        QFont font;
        font.setPointSize(13);
        font.setBold(false);
        font.setWeight(50);
        font.setKerning(true);
        comboBox_7->setFont(font);
        comboBox_7->setEditable(true);

        horizontalLayout_4->addWidget(comboBox_7);

        pushButton_11 = new QPushButton(SipPhone);
        pushButton_11->setObjectName(QString::fromUtf8("pushButton_11"));
        pushButton_11->setMaximumSize(QSize(30, 16777215));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/skins/default/phone_32.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_11->setIcon(icon5);
        pushButton_11->setAutoDefault(false);
        pushButton_11->setDefault(false);
        pushButton_11->setFlat(false);

        horizontalLayout_4->addWidget(pushButton_11);


        verticalLayout_5->addLayout(horizontalLayout_4);

        widget = new QWidget(SipPhone);
        widget->setObjectName(QString::fromUtf8("widget"));
        gridLayout = new QGridLayout(widget);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        toolButton_15 = new QToolButton(widget);
        toolButton_15->setObjectName(QString::fromUtf8("toolButton_15"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/skins/default/digit1.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_15->setIcon(icon6);
        toolButton_15->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_15, 0, 1, 1, 1);

        toolButton_16 = new QToolButton(widget);
        toolButton_16->setObjectName(QString::fromUtf8("toolButton_16"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/skins/default/digit2.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_16->setIcon(icon7);
        toolButton_16->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_16, 0, 2, 1, 1);

        toolButton_17 = new QToolButton(widget);
        toolButton_17->setObjectName(QString::fromUtf8("toolButton_17"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/skins/default/digit3.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_17->setIcon(icon8);
        toolButton_17->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_17, 0, 3, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(25, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 1, 0, 1, 1);

        toolButton_18 = new QToolButton(widget);
        toolButton_18->setObjectName(QString::fromUtf8("toolButton_18"));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/skins/default/digit4.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_18->setIcon(icon9);
        toolButton_18->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_18, 1, 1, 1, 1);

        toolButton_19 = new QToolButton(widget);
        toolButton_19->setObjectName(QString::fromUtf8("toolButton_19"));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/skins/default/digit5.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_19->setIcon(icon10);
        toolButton_19->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_19, 1, 2, 1, 1);

        toolButton_20 = new QToolButton(widget);
        toolButton_20->setObjectName(QString::fromUtf8("toolButton_20"));
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/skins/default/digit6.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_20->setIcon(icon11);
        toolButton_20->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_20, 1, 3, 1, 1);

        toolButton_21 = new QToolButton(widget);
        toolButton_21->setObjectName(QString::fromUtf8("toolButton_21"));
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/skins/default/digit7.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_21->setIcon(icon12);
        toolButton_21->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_21, 2, 1, 1, 1);

        toolButton_22 = new QToolButton(widget);
        toolButton_22->setObjectName(QString::fromUtf8("toolButton_22"));
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/skins/default/digit8.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_22->setIcon(icon13);
        toolButton_22->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_22, 2, 2, 1, 1);

        toolButton_23 = new QToolButton(widget);
        toolButton_23->setObjectName(QString::fromUtf8("toolButton_23"));
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/skins/default/digit9.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_23->setIcon(icon14);
        toolButton_23->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_23, 2, 3, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(75, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 2, 4, 1, 1);

        toolButton_24 = new QToolButton(widget);
        toolButton_24->setObjectName(QString::fromUtf8("toolButton_24"));
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/skins/default/digitstar.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_24->setIcon(icon15);
        toolButton_24->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_24, 3, 1, 1, 1);

        toolButton_14 = new QToolButton(widget);
        toolButton_14->setObjectName(QString::fromUtf8("toolButton_14"));
        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/skins/default/digit0.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_14->setIcon(icon16);
        toolButton_14->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_14, 3, 2, 1, 1);

        toolButton_25 = new QToolButton(widget);
        toolButton_25->setObjectName(QString::fromUtf8("toolButton_25"));
        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/skins/default/digitpound.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_25->setIcon(icon17);
        toolButton_25->setIconSize(QSize(50, 22));

        gridLayout->addWidget(toolButton_25, 3, 3, 1, 1);

        comboBox_2 = new QComboBox(widget);
        comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));

        gridLayout->addWidget(comboBox_2, 0, 4, 1, 1);

        comboBox = new QComboBox(widget);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));

        gridLayout->addWidget(comboBox, 1, 4, 1, 1);

        comboBox_3 = new QComboBox(widget);
        comboBox_3->setObjectName(QString::fromUtf8("comboBox_3"));

        gridLayout->addWidget(comboBox_3, 3, 4, 1, 1);

        checkBox = new QCheckBox(widget);
        checkBox->setObjectName(QString::fromUtf8("checkBox"));

        gridLayout->addWidget(checkBox, 3, 0, 1, 1);


        verticalLayout_5->addWidget(widget);

        widget_2 = new QWidget(SipPhone);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        verticalLayout = new QVBoxLayout(widget_2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        label_10 = new QLabel(widget_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setPixmap(QPixmap(QString::fromUtf8(":/skins/default/speaker.png")));

        horizontalLayout_9->addWidget(label_10);

        label = new QLabel(widget_2);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_9->addWidget(label);

        label_11 = new QLabel(widget_2);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        horizontalLayout_9->addWidget(label_11);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_5);

        toolButton = new QToolButton(widget_2);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));
        toolButton->setCheckable(true);
        toolButton->setChecked(true);
        toolButton->setAutoRaise(true);
        toolButton->setArrowType(Qt::DownArrow);

        horizontalLayout_9->addWidget(toolButton);

        label_12 = new QLabel(widget_2);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setPixmap(QPixmap(QString::fromUtf8(":/skins/default/Un-mute_Alpha_down_16x16_s1.png")));
        label_12->setScaledContents(false);

        horizontalLayout_9->addWidget(label_12);


        verticalLayout->addLayout(horizontalLayout_9);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        verticalSlider = new QSlider(widget_2);
        verticalSlider->setObjectName(QString::fromUtf8("verticalSlider"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(verticalSlider->sizePolicy().hasHeightForWidth());
        verticalSlider->setSizePolicy(sizePolicy2);
        verticalSlider->setMaximumSize(QSize(16777215, 50));
        verticalSlider->setSingleStep(5);
        verticalSlider->setValue(99);
        verticalSlider->setOrientation(Qt::Vertical);
        verticalSlider->setTickPosition(QSlider::TicksBothSides);

        horizontalLayout_8->addWidget(verticalSlider);

        label_7 = new QLabel(widget_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setMaximumSize(QSize(35, 16777215));
        label_7->setPixmap(QPixmap(QString::fromUtf8(":/skins/default/user.png")));
        label_7->setAlignment(Qt::AlignCenter);

        horizontalLayout_8->addWidget(label_7);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        label_5 = new QLabel(widget_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        verticalLayout_4->addWidget(label_5);

        label_6 = new QLabel(widget_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        verticalLayout_4->addWidget(label_6);


        horizontalLayout_8->addLayout(verticalLayout_4);

        verticalSlider_2 = new QSlider(widget_2);
        verticalSlider_2->setObjectName(QString::fromUtf8("verticalSlider_2"));
        sizePolicy2.setHeightForWidth(verticalSlider_2->sizePolicy().hasHeightForWidth());
        verticalSlider_2->setSizePolicy(sizePolicy2);
        verticalSlider_2->setMinimumSize(QSize(0, 50));
        verticalSlider_2->setMaximumSize(QSize(16777215, 50));
        verticalSlider_2->setSingleStep(5);
        verticalSlider_2->setValue(99);
        verticalSlider_2->setOrientation(Qt::Vertical);
        verticalSlider_2->setInvertedAppearance(false);
        verticalSlider_2->setInvertedControls(false);
        verticalSlider_2->setTickPosition(QSlider::TicksBothSides);

        horizontalLayout_8->addWidget(verticalSlider_2);


        verticalLayout->addLayout(horizontalLayout_8);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        toolButton_5 = new QToolButton(widget_2);
        toolButton_5->setObjectName(QString::fromUtf8("toolButton_5"));
        toolButton_5->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout_7->addWidget(toolButton_5);

        toolButton_12 = new QToolButton(widget_2);
        toolButton_12->setObjectName(QString::fromUtf8("toolButton_12"));
        toolButton_12->setIcon(icon3);

        horizontalLayout_7->addWidget(toolButton_12);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_4);

        toolButton_10 = new QToolButton(widget_2);
        toolButton_10->setObjectName(QString::fromUtf8("toolButton_10"));
        QIcon icon18;
        icon18.addFile(QString::fromUtf8(":/skins/default/hangup_20.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_10->setIcon(icon18);

        horizontalLayout_7->addWidget(toolButton_10);


        verticalLayout->addLayout(horizontalLayout_7);


        verticalLayout_5->addWidget(widget_2);

        tabWidget = new QTabWidget(SipPhone);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setContextMenuPolicy(Qt::DefaultContextMenu);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setDocumentMode(true);
        tabWidget->setTabsClosable(false);
        tabWidget->setMovable(true);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_2 = new QVBoxLayout(tab);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        toolButton_6 = new QToolButton(tab);
        toolButton_6->setObjectName(QString::fromUtf8("toolButton_6"));
        QIcon icon19;
        icon19.addFile(QString::fromUtf8(":/skins/default/addcontact.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_6->setIcon(icon19);
        toolButton_6->setPopupMode(QToolButton::MenuButtonPopup);
        toolButton_6->setArrowType(Qt::NoArrow);

        horizontalLayout_5->addWidget(toolButton_6);

        comboBox_8 = new QComboBox(tab);
        comboBox_8->setObjectName(QString::fromUtf8("comboBox_8"));
        comboBox_8->setEditable(true);

        horizontalLayout_5->addWidget(comboBox_8);

        toolButton_7 = new QToolButton(tab);
        toolButton_7->setObjectName(QString::fromUtf8("toolButton_7"));

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
        treeView->setAnimated(true);

        verticalLayout_2->addWidget(treeView);

        QIcon icon20;
        icon20.addFile(QString::fromUtf8(":/skins/default/book_32.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabWidget->addTab(tab, icon20, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayout_3 = new QVBoxLayout(tab_2);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        comboBox_9 = new QComboBox(tab_2);
        comboBox_9->setObjectName(QString::fromUtf8("comboBox_9"));
        sizePolicy1.setHeightForWidth(comboBox_9->sizePolicy().hasHeightForWidth());
        comboBox_9->setSizePolicy(sizePolicy1);
        comboBox_9->setEditable(true);

        horizontalLayout_6->addWidget(comboBox_9);

        comboBox_10 = new QComboBox(tab_2);
        comboBox_10->setObjectName(QString::fromUtf8("comboBox_10"));
        sizePolicy.setHeightForWidth(comboBox_10->sizePolicy().hasHeightForWidth());
        comboBox_10->setSizePolicy(sizePolicy);

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

        tabWidget->addTab(tab_2, QString());

        verticalLayout_5->addWidget(tabWidget);

        listWidget = new QListWidget(SipPhone);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        verticalLayout_5->addWidget(listWidget);


        retranslateUi(SipPhone);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SipPhone);
    } // setupUi

    void retranslateUi(QWidget *SipPhone)
    {
        SipPhone->setWindowTitle(QApplication::translate("SipPhone", "Kitphone for SIP", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("SipPhone", "\346\240\207\345\207\206SIP\345\215\217\350\256\256\346\250\241\345\274\217\357\274\214\350\257\255\351\237\263\347\233\264\351\200\232+P2P\347\251\277\351\200\217.", 0, QApplication::UnicodeUTF8));
        pushButton_5->setText(QApplication::translate("SipPhone", "Manage Account ...", 0, QApplication::UnicodeUTF8));
        pushButton_6->setText(QApplication::translate("SipPhone", "Preferences", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        label_4->setToolTip(QApplication::translate("SipPhone", "This account online status icons.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_4->setText(QString());
        comboBox_5->setItemText(0, QApplication::translate("SipPhone", "Avalible", 0, QApplication::UnicodeUTF8));
        comboBox_5->setItemText(1, QApplication::translate("SipPhone", "Offline", 0, QApplication::UnicodeUTF8));

#ifndef QT_NO_TOOLTIP
        comboBox_5->setToolTip(QApplication::translate("SipPhone", "This account on line status.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        comboBox_6->clear();
        comboBox_6->insertItems(0, QStringList()
         << QApplication::translate("SipPhone", "-- NONE --", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        comboBox_6->setToolTip(QApplication::translate("SipPhone", "sip user uri: such as anony@sip2sips.org:5060", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        toolButton_2->setToolTip(QApplication::translate("SipPhone", "Open sound settings.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        toolButton_2->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        toolButton_3->setToolTip(QApplication::translate("SipPhone", "Open video camro settings.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        toolButton_3->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_4->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        comboBox_7->clear();
        comboBox_7->insertItems(0, QStringList()
         << QApplication::translate("SipPhone", "*013800138000", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SipPhone", "*01068730066", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SipPhone", "*01096196", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        comboBox_7->setToolTip(QApplication::translate("SipPhone", "phone number: such as 123-12344556", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushButton_11->setText(QString());
        toolButton_15->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_16->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_17->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_18->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_19->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_20->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_21->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_22->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_23->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_24->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_14->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_25->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        comboBox_2->clear();
        comboBox_2->insertItems(0, QStringList()
         << QApplication::translate("SipPhone", "UDP", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SipPhone", "TCP", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SipPhone", "TLS", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SipPhone", "IPV6", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SipPhone", "UDP6", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SipPhone", "TCP6", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        comboBox->setToolTip(QApplication::translate("SipPhone", "\350\257\255\351\237\263\347\274\226\347\240\201", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        comboBox_3->clear();
        comboBox_3->insertItems(0, QStringList()
         << QApplication::translate("SipPhone", "STUN", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SipPhone", "TURN", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SipPhone", "ICE", 0, QApplication::UnicodeUTF8)
        );
        checkBox->setText(QApplication::translate("SipPhone", "TLS", 0, QApplication::UnicodeUTF8));
        label_10->setText(QString());
        label->setText(QString());
        label_11->setText(QApplication::translate("SipPhone", "call status message here", 0, QApplication::UnicodeUTF8));
        toolButton->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        label_12->setText(QString());
        label_7->setText(QString());
        label_5->setText(QApplication::translate("SipPhone", "call no here................", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("SipPhone", "Group: Others", 0, QApplication::UnicodeUTF8));
        toolButton_5->setText(QApplication::translate("SipPhone", "Hold", 0, QApplication::UnicodeUTF8));
        toolButton_12->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_10->setText(QApplication::translate("SipPhone", "End", 0, QApplication::UnicodeUTF8));
        toolButton_6->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_7->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        toolButton_8->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("SipPhone", "\350\201\224\347\263\273\344\272\272", 0, QApplication::UnicodeUTF8));
        comboBox_10->clear();
        comboBox_10->insertItems(0, QStringList()
         << QApplication::translate("SipPhone", "All", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SipPhone", "Missed", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SipPhone", "Received", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SipPhone", "Deleted", 0, QApplication::UnicodeUTF8)
        );
        toolButton_9->setText(QApplication::translate("SipPhone", "...", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("SipPhone", "\345\221\274\345\217\253\345\216\206\345\217\262", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SipPhone: public Ui_SipPhone {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIPPHONE_H
