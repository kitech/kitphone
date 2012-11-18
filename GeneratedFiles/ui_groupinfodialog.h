/********************************************************************************
** Form generated from reading UI file 'groupinfodialog.ui'
**
** Created: Mon Sep 19 01:51:19 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GROUPINFODIALOG_H
#define UI_GROUPINFODIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_GroupInfoDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_2;
    QLabel *label;
    QComboBox *comboBox;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *GroupInfoDialog)
    {
        if (GroupInfoDialog->objectName().isEmpty())
            GroupInfoDialog->setObjectName(QString::fromUtf8("GroupInfoDialog"));
        GroupInfoDialog->resize(290, 144);
        verticalLayout = new QVBoxLayout(GroupInfoDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_2 = new QLabel(GroupInfoDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        label = new QLabel(GroupInfoDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        verticalLayout->addWidget(label);

        comboBox = new QComboBox(GroupInfoDialog);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setEditable(true);

        verticalLayout->addWidget(comboBox);

        verticalSpacer = new QSpacerItem(20, 17, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(GroupInfoDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(GroupInfoDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), GroupInfoDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), GroupInfoDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(GroupInfoDialog);
    } // setupUi

    void retranslateUi(QDialog *GroupInfoDialog)
    {
        GroupInfoDialog->setWindowTitle(QApplication::translate("GroupInfoDialog", "\346\226\260\350\201\224\347\263\273\344\272\272\347\273\204", 0, QApplication::UnicodeUTF8));
        label_2->setText(QString());
        label->setText(QApplication::translate("GroupInfoDialog", "\350\276\223\345\205\245\346\226\260\350\201\224\347\263\273\344\272\272\347\273\204\345\220\215\347\247\260\357\274\232", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class GroupInfoDialog: public Ui_GroupInfoDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GROUPINFODIALOG_H
