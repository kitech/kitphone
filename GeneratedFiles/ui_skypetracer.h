/********************************************************************************
** Form generated from reading UI file 'skypetracer.ui'
**
** Created: Mon Sep 19 01:51:18 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SKYPETRACER_H
#define UI_SKYPETRACER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QTextBrowser>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SkypeTracer
{
public:
    QVBoxLayout *verticalLayout_2;
    QSplitter *splitter;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QTextBrowser *textBrowser;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *comboBox;
    QToolButton *toolButton;
    QPushButton *pushButton_8;
    QHBoxLayout *horizontalLayout;
    QToolButton *toolButton_2;
    QToolButton *toolButton_3;
    QToolButton *toolButton_4;
    QToolButton *toolButton_5;
    QToolButton *toolButton_6;
    QToolButton *toolButton_7;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_7;
    QListWidget *listWidget;

    void setupUi(QDialog *SkypeTracer)
    {
        if (SkypeTracer->objectName().isEmpty())
            SkypeTracer->setObjectName(QString::fromUtf8("SkypeTracer"));
        SkypeTracer->resize(767, 545);
        verticalLayout_2 = new QVBoxLayout(SkypeTracer);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        splitter = new QSplitter(SkypeTracer);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        layoutWidget = new QWidget(splitter);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        textBrowser = new QTextBrowser(layoutWidget);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));

        verticalLayout->addWidget(textBrowser);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        comboBox = new QComboBox(layoutWidget);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setEditable(true);

        horizontalLayout_2->addWidget(comboBox);

        toolButton = new QToolButton(layoutWidget);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));

        horizontalLayout_2->addWidget(toolButton);

        pushButton_8 = new QPushButton(layoutWidget);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton_8->sizePolicy().hasHeightForWidth());
        pushButton_8->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(pushButton_8);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        toolButton_2 = new QToolButton(layoutWidget);
        toolButton_2->setObjectName(QString::fromUtf8("toolButton_2"));

        horizontalLayout->addWidget(toolButton_2);

        toolButton_3 = new QToolButton(layoutWidget);
        toolButton_3->setObjectName(QString::fromUtf8("toolButton_3"));

        horizontalLayout->addWidget(toolButton_3);

        toolButton_4 = new QToolButton(layoutWidget);
        toolButton_4->setObjectName(QString::fromUtf8("toolButton_4"));

        horizontalLayout->addWidget(toolButton_4);

        toolButton_5 = new QToolButton(layoutWidget);
        toolButton_5->setObjectName(QString::fromUtf8("toolButton_5"));

        horizontalLayout->addWidget(toolButton_5);

        toolButton_6 = new QToolButton(layoutWidget);
        toolButton_6->setObjectName(QString::fromUtf8("toolButton_6"));

        horizontalLayout->addWidget(toolButton_6);

        toolButton_7 = new QToolButton(layoutWidget);
        toolButton_7->setObjectName(QString::fromUtf8("toolButton_7"));

        horizontalLayout->addWidget(toolButton_7);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_7 = new QPushButton(layoutWidget);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));

        horizontalLayout->addWidget(pushButton_7);


        verticalLayout->addLayout(horizontalLayout);

        splitter->addWidget(layoutWidget);
        listWidget = new QListWidget(splitter);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        splitter->addWidget(listWidget);

        verticalLayout_2->addWidget(splitter);


        retranslateUi(SkypeTracer);

        QMetaObject::connectSlotsByName(SkypeTracer);
    } // setupUi

    void retranslateUi(QDialog *SkypeTracer)
    {
        SkypeTracer->setWindowTitle(QApplication::translate("SkypeTracer", "Skype Protocol Tracer", 0, QApplication::UnicodeUTF8));
        toolButton->setText(QApplication::translate("SkypeTracer", "...", 0, QApplication::UnicodeUTF8));
        pushButton_8->setText(QApplication::translate("SkypeTracer", "PushButton", 0, QApplication::UnicodeUTF8));
        toolButton_2->setText(QApplication::translate("SkypeTracer", "..A..", 0, QApplication::UnicodeUTF8));
        toolButton_3->setText(QApplication::translate("SkypeTracer", "..B..", 0, QApplication::UnicodeUTF8));
        toolButton_4->setText(QApplication::translate("SkypeTracer", "..C..", 0, QApplication::UnicodeUTF8));
        toolButton_5->setText(QApplication::translate("SkypeTracer", "..D..", 0, QApplication::UnicodeUTF8));
        toolButton_6->setText(QApplication::translate("SkypeTracer", "..E..", 0, QApplication::UnicodeUTF8));
        toolButton_7->setText(QApplication::translate("SkypeTracer", "..F..", 0, QApplication::UnicodeUTF8));
        pushButton_7->setText(QApplication::translate("SkypeTracer", "z", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = listWidget->isSortingEnabled();
        listWidget->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = listWidget->item(0);
        ___qlistwidgetitem->setText(QApplication::translate("SkypeTracer", "ALTER APPLICATION <appname> CONNECT <username>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem1 = listWidget->item(1);
        ___qlistwidgetitem1->setText(QApplication::translate("SkypeTracer", "ALTER APPLICATION <appname> DATAGRAM <username>:<id> <text>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem2 = listWidget->item(2);
        ___qlistwidgetitem2->setText(QApplication::translate("SkypeTracer", "ALTER APPLICATION <appname> DISCONNECT <username>:<id>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem3 = listWidget->item(3);
        ___qlistwidgetitem3->setText(QApplication::translate("SkypeTracer", "ALTER APPLICATION <appname> READ <username>:<id>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem4 = listWidget->item(4);
        ___qlistwidgetitem4->setText(QApplication::translate("SkypeTracer", "ALTER APPLICATION <appname> WRITE <username>:<id> <text>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem5 = listWidget->item(5);
        ___qlistwidgetitem5->setText(QApplication::translate("SkypeTracer", "ALTER CALL <id> <status>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem6 = listWidget->item(6);
        ___qlistwidgetitem6->setText(QApplication::translate("SkypeTracer", "ALTER CALL <id> SET_INPUT PORT=\"<port>\"", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem7 = listWidget->item(7);
        ___qlistwidgetitem7->setText(QApplication::translate("SkypeTracer", "ALTER CALL <id> SET_OUTPUT PORT=\"<port>\"", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem8 = listWidget->item(8);
        ___qlistwidgetitem8->setText(QApplication::translate("SkypeTracer", "ALTER CHAT <chat_id> ADDMEMBERS <target>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem9 = listWidget->item(9);
        ___qlistwidgetitem9->setText(QApplication::translate("SkypeTracer", "ALTER CHAT <chat_id> LEAVE", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem10 = listWidget->item(10);
        ___qlistwidgetitem10->setText(QApplication::translate("SkypeTracer", "ALTER CHAT <chat_id> SETTOPIC <topic>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem11 = listWidget->item(11);
        ___qlistwidgetitem11->setText(QApplication::translate("SkypeTracer", "ALTER GROUP <id> ADDUSER <userhandle|PSTN>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem12 = listWidget->item(12);
        ___qlistwidgetitem12->setText(QApplication::translate("SkypeTracer", "ALTER GROUP <id> REMOVEUSER <userhandle|PSTN>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem13 = listWidget->item(13);
        ___qlistwidgetitem13->setText(QApplication::translate("SkypeTracer", "ALTER VOICEMAIL <id> <action>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem14 = listWidget->item(14);
        ___qlistwidgetitem14->setText(QApplication::translate("SkypeTracer", "APPLICATION <appname> <property> <value>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem15 = listWidget->item(15);
        ___qlistwidgetitem15->setText(QApplication::translate("SkypeTracer", "BTN_PRESSED <btn>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem16 = listWidget->item(16);
        ___qlistwidgetitem16->setText(QApplication::translate("SkypeTracer", "BTN_RELEASED <btn>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem17 = listWidget->item(17);
        ___qlistwidgetitem17->setText(QApplication::translate("SkypeTracer", "CALL <id>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem18 = listWidget->item(18);
        ___qlistwidgetitem18->setText(QApplication::translate("SkypeTracer", "CHAT CREATE <target>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem19 = listWidget->item(19);
        ___qlistwidgetitem19->setText(QApplication::translate("SkypeTracer", "CHATMESSAGE <chat_id> <message>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem20 = listWidget->item(20);
        ___qlistwidgetitem20->setText(QApplication::translate("SkypeTracer", "CLEAR CALLHISTORY", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem21 = listWidget->item(21);
        ___qlistwidgetitem21->setText(QApplication::translate("SkypeTracer", "CLEAR CHATHISTORY", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem22 = listWidget->item(22);
        ___qlistwidgetitem22->setText(QApplication::translate("SkypeTracer", "CLEAR VOICEMAILHISTORY", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem23 = listWidget->item(23);
        ___qlistwidgetitem23->setText(QApplication::translate("SkypeTracer", "CONTACTS FOCUSED", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem24 = listWidget->item(24);
        ___qlistwidgetitem24->setText(QApplication::translate("SkypeTracer", "CONTACTS FOCUSED <username>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem25 = listWidget->item(25);
        ___qlistwidgetitem25->setText(QApplication::translate("SkypeTracer", "CREATE APPLICATION <appname>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem26 = listWidget->item(26);
        ___qlistwidgetitem26->setText(QApplication::translate("SkypeTracer", "CREATE GROUP <Group|id>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem27 = listWidget->item(27);
        ___qlistwidgetitem27->setText(QApplication::translate("SkypeTracer", "DELETE APPLICATION <appname>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem28 = listWidget->item(28);
        ___qlistwidgetitem28->setText(QApplication::translate("SkypeTracer", "DELETE GROUP <id>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem29 = listWidget->item(29);
        ___qlistwidgetitem29->setText(QApplication::translate("SkypeTracer", "GET AGC", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem30 = listWidget->item(30);
        ___qlistwidgetitem30->setText(QApplication::translate("SkypeTracer", "GET AUDIO_IN", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem31 = listWidget->item(31);
        ___qlistwidgetitem31->setText(QApplication::translate("SkypeTracer", "GET AUDIO_OUT", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem32 = listWidget->item(32);
        ___qlistwidgetitem32->setText(QApplication::translate("SkypeTracer", "GET CALL <id> <property>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem33 = listWidget->item(33);
        ___qlistwidgetitem33->setText(QApplication::translate("SkypeTracer", "GET CHAT <chat_id> <property>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem34 = listWidget->item(34);
        ___qlistwidgetitem34->setText(QApplication::translate("SkypeTracer", "GET CHAT <chat_id> CHATMESSAGES", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem35 = listWidget->item(35);
        ___qlistwidgetitem35->setText(QApplication::translate("SkypeTracer", "GET CHAT <chat_id> RECENTCHATMESSAGES", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem36 = listWidget->item(36);
        ___qlistwidgetitem36->setText(QApplication::translate("SkypeTracer", "GET CONNSTATUS", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem37 = listWidget->item(37);
        ___qlistwidgetitem37->setText(QApplication::translate("SkypeTracer", "GET CURRENTUSERHANDLE", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem38 = listWidget->item(38);
        ___qlistwidgetitem38->setText(QApplication::translate("SkypeTracer", "GET GROUP <id> property", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem39 = listWidget->item(39);
        ___qlistwidgetitem39->setText(QApplication::translate("SkypeTracer", "GET MESSAGE <id> <peroperty>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem40 = listWidget->item(40);
        ___qlistwidgetitem40->setText(QApplication::translate("SkypeTracer", "GET MUTE", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem41 = listWidget->item(41);
        ___qlistwidgetitem41->setText(QApplication::translate("SkypeTracer", "GET PRIVILEGE <user_privilege>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem42 = listWidget->item(42);
        ___qlistwidgetitem42->setText(QApplication::translate("SkypeTracer", "GET PROFILE <profile_property>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem43 = listWidget->item(43);
        ___qlistwidgetitem43->setText(QApplication::translate("SkypeTracer", "GET PROFILE <profile>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem44 = listWidget->item(44);
        ___qlistwidgetitem44->setText(QApplication::translate("SkypeTracer", "GET RINGER", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem45 = listWidget->item(45);
        ___qlistwidgetitem45->setText(QApplication::translate("SkypeTracer", "GET SKYPEVERSION", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem46 = listWidget->item(46);
        ___qlistwidgetitem46->setText(QApplication::translate("SkypeTracer", "GET USER <username> <property>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem47 = listWidget->item(47);
        ___qlistwidgetitem47->setText(QApplication::translate("SkypeTracer", "GET USERSTATUS", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem48 = listWidget->item(48);
        ___qlistwidgetitem48->setText(QApplication::translate("SkypeTracer", "GET VIDEO_IN", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem49 = listWidget->item(49);
        ___qlistwidgetitem49->setText(QApplication::translate("SkypeTracer", "HOOK <ON|OFF>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem50 = listWidget->item(50);
        ___qlistwidgetitem50->setText(QApplication::translate("SkypeTracer", "MESSAGE <id> <property> <value>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem51 = listWidget->item(51);
        ___qlistwidgetitem51->setText(QApplication::translate("SkypeTracer", "MINIMIZE", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem52 = listWidget->item(52);
        ___qlistwidgetitem52->setText(QApplication::translate("SkypeTracer", "MUTE <ON|OFF>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem53 = listWidget->item(53);
        ___qlistwidgetitem53->setText(QApplication::translate("SkypeTracer", "OPEN ADDAFRIEND <username>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem54 = listWidget->item(54);
        ___qlistwidgetitem54->setText(QApplication::translate("SkypeTracer", "OPEN AUTHORIZATION <username>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem55 = listWidget->item(55);
        ___qlistwidgetitem55->setText(QApplication::translate("SkypeTracer", "OPEN BLOCKEDUSERS", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem56 = listWidget->item(56);
        ___qlistwidgetitem56->setText(QApplication::translate("SkypeTracer", "OPEN CALLHISTORY", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem57 = listWidget->item(57);
        ___qlistwidgetitem57->setText(QApplication::translate("SkypeTracer", "OPEN CHAT <chat_id>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem58 = listWidget->item(58);
        ___qlistwidgetitem58->setText(QApplication::translate("SkypeTracer", "OPEN CONTACTS", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem59 = listWidget->item(59);
        ___qlistwidgetitem59->setText(QApplication::translate("SkypeTracer", "OPEN DIALPAD", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem60 = listWidget->item(60);
        ___qlistwidgetitem60->setText(QApplication::translate("SkypeTracer", "OPEN FILETRANSFER <username> IN <folder>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem61 = listWidget->item(61);
        ___qlistwidgetitem61->setText(QApplication::translate("SkypeTracer", "OPEN GETTINGSTARTED", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem62 = listWidget->item(62);
        ___qlistwidgetitem62->setText(QApplication::translate("SkypeTracer", "OPEN IM <username> <message>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem63 = listWidget->item(63);
        ___qlistwidgetitem63->setText(QApplication::translate("SkypeTracer", "OPEN IMPORTCONTACTS", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem64 = listWidget->item(64);
        ___qlistwidgetitem64->setText(QApplication::translate("SkypeTracer", "OPEN OPTIONS <page>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem65 = listWidget->item(65);
        ___qlistwidgetitem65->setText(QApplication::translate("SkypeTracer", "OPEN PROFILE", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem66 = listWidget->item(66);
        ___qlistwidgetitem66->setText(QApplication::translate("SkypeTracer", "OPEN SEARCH", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem67 = listWidget->item(67);
        ___qlistwidgetitem67->setText(QApplication::translate("SkypeTracer", "OPEN SENDCONTACTS", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem68 = listWidget->item(68);
        ___qlistwidgetitem68->setText(QApplication::translate("SkypeTracer", "OPEN USERINFO <username>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem69 = listWidget->item(69);
        ___qlistwidgetitem69->setText(QApplication::translate("SkypeTracer", "OPEN VIDEOTEST", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem70 = listWidget->item(70);
        ___qlistwidgetitem70->setText(QApplication::translate("SkypeTracer", "OPEN VOICEMAIL <id>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem71 = listWidget->item(71);
        ___qlistwidgetitem71->setText(QApplication::translate("SkypeTracer", "PING", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem72 = listWidget->item(72);
        ___qlistwidgetitem72->setText(QApplication::translate("SkypeTracer", "PROTOCOL <version>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem73 = listWidget->item(73);
        ___qlistwidgetitem73->setText(QApplication::translate("SkypeTracer", "SEARCH ACTIVECALLS", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem74 = listWidget->item(74);
        ___qlistwidgetitem74->setText(QApplication::translate("SkypeTracer", "SEARCH BOOKMARKEDCHATS", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem75 = listWidget->item(75);
        ___qlistwidgetitem75->setText(QApplication::translate("SkypeTracer", "SEARCH CALLS <target>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem76 = listWidget->item(76);
        ___qlistwidgetitem76->setText(QApplication::translate("SkypeTracer", "SEARCH CHATMESSAGES <username>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem77 = listWidget->item(77);
        ___qlistwidgetitem77->setText(QApplication::translate("SkypeTracer", "SEARCH CHATS", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem78 = listWidget->item(78);
        ___qlistwidgetitem78->setText(QApplication::translate("SkypeTracer", "SEARCH FRIENDS", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem79 = listWidget->item(79);
        ___qlistwidgetitem79->setText(QApplication::translate("SkypeTracer", "SEARCH GROUPS", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem80 = listWidget->item(80);
        ___qlistwidgetitem80->setText(QApplication::translate("SkypeTracer", "SEARCH MESSAGES <target>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem81 = listWidget->item(81);
        ___qlistwidgetitem81->setText(QApplication::translate("SkypeTracer", "SEARCH MISSEDCALLS", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem82 = listWidget->item(82);
        ___qlistwidgetitem82->setText(QApplication::translate("SkypeTracer", "SEARCH MISSEDCHATMESSAGES", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem83 = listWidget->item(83);
        ___qlistwidgetitem83->setText(QApplication::translate("SkypeTracer", "SEARCH RECENTCHATS", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem84 = listWidget->item(84);
        ___qlistwidgetitem84->setText(QApplication::translate("SkypeTracer", "SEARCH USERS <target>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem85 = listWidget->item(85);
        ___qlistwidgetitem85->setText(QApplication::translate("SkypeTracer", "SEARCH USERSWAITINGMYAUTHORIZATION", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem86 = listWidget->item(86);
        ___qlistwidgetitem86->setText(QApplication::translate("SkypeTracer", "SEARCH VOICEMAILS", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem87 = listWidget->item(87);
        ___qlistwidgetitem87->setText(QApplication::translate("SkypeTracer", "SET AUDIO_IN <device_name>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem88 = listWidget->item(88);
        ___qlistwidgetitem88->setText(QApplication::translate("SkypeTracer", "SET AUDIO_OUT <device_name>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem89 = listWidget->item(89);
        ___qlistwidgetitem89->setText(QApplication::translate("SkypeTracer", "SET CALL <id> property", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem90 = listWidget->item(90);
        ___qlistwidgetitem90->setText(QApplication::translate("SkypeTracer", "SET CHATMESSAGE <id> SEEN", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem91 = listWidget->item(91);
        ___qlistwidgetitem91->setText(QApplication::translate("SkypeTracer", "SET MESSAGE <id> SEEN", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem92 = listWidget->item(92);
        ___qlistwidgetitem92->setText(QApplication::translate("SkypeTracer", "SET MUTE <ON|OFF>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem93 = listWidget->item(93);
        ___qlistwidgetitem93->setText(QApplication::translate("SkypeTracer", "SET PROFILE <profile_property> <value>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem94 = listWidget->item(94);
        ___qlistwidgetitem94->setText(QApplication::translate("SkypeTracer", "SET PROFILE <profile>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem95 = listWidget->item(95);
        ___qlistwidgetitem95->setText(QApplication::translate("SkypeTracer", "SET PROFILE MOOD_TEXT <text>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem96 = listWidget->item(96);
        ___qlistwidgetitem96->setText(QApplication::translate("SkypeTracer", "SET RINGER <device_name>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem97 = listWidget->item(97);
        ___qlistwidgetitem97->setText(QApplication::translate("SkypeTracer", "SET USER <username> <property> <value>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem98 = listWidget->item(98);
        ___qlistwidgetitem98->setText(QApplication::translate("SkypeTracer", "SET USERSTATUS <value>", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem99 = listWidget->item(99);
        ___qlistwidgetitem99->setText(QApplication::translate("SkypeTracer", "SET SILENT MODE <ON|OFF>", 0, QApplication::UnicodeUTF8));
        listWidget->setSortingEnabled(__sortingEnabled);

    } // retranslateUi

};

namespace Ui {
    class SkypeTracer: public Ui_SkypeTracer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SKYPETRACER_H
