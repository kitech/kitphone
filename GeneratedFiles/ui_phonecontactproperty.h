/********************************************************************************
** Form generated from reading UI file 'phonecontactproperty.ui'
**
** Created: Mon Sep 19 01:51:19 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PHONECONTACTPROPERTY_H
#define UI_PHONECONTACTPROPERTY_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_PhoneContactProperty
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QLabel *label_4;
    QGridLayout *gridLayout;
    QLabel *label;
    QComboBox *comboBox;
    QLabel *label_3;
    QComboBox *comboBox_3;
    QLabel *label_2;
    QSpacerItem *verticalSpacer;
    QLineEdit *lineEdit;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QLabel *label_5;
    QComboBox *comboBox_4;
    QLabel *label_9;
    QLabel *label_6;
    QLabel *label_10;
    QLabel *label_7;
    QComboBox *comboBox_5;
    QLabel *label_8;
    QSpacerItem *verticalSpacer_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *PhoneContactProperty)
    {
        if (PhoneContactProperty->objectName().isEmpty())
            PhoneContactProperty->setObjectName(QString::fromUtf8("PhoneContactProperty"));
        PhoneContactProperty->resize(510, 330);
        verticalLayout = new QVBoxLayout(PhoneContactProperty);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(PhoneContactProperty);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy);
        label_4->setPixmap(QPixmap(QString::fromUtf8(":/skins/default/book_32.png")));
        label_4->setScaledContents(false);

        horizontalLayout->addWidget(label_4);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        comboBox = new QComboBox(groupBox);
        comboBox->setObjectName(QString::fromUtf8("comboBox"));
        comboBox->setEditable(true);

        gridLayout->addWidget(comboBox, 0, 1, 2, 1);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 0, 2, 1, 1);

        comboBox_3 = new QComboBox(groupBox);
        comboBox_3->setObjectName(QString::fromUtf8("comboBox_3"));
        comboBox_3->setEditable(false);

        gridLayout->addWidget(comboBox_3, 0, 3, 2, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 2, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 3, 1, 1, 1);

        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
        lineEdit->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(lineEdit, 2, 1, 1, 1);


        horizontalLayout->addLayout(gridLayout);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(PhoneContactProperty);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        sizePolicy.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy);

        gridLayout_2->addWidget(label_5, 0, 0, 1, 1);

        comboBox_4 = new QComboBox(groupBox_2);
        comboBox_4->setObjectName(QString::fromUtf8("comboBox_4"));
        comboBox_4->setEditable(true);

        gridLayout_2->addWidget(comboBox_4, 0, 1, 1, 1);

        label_9 = new QLabel(groupBox_2);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout_2->addWidget(label_9, 0, 2, 1, 1);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout_2->addWidget(label_6, 1, 0, 1, 1);

        label_10 = new QLabel(groupBox_2);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout_2->addWidget(label_10, 1, 2, 1, 1);

        label_7 = new QLabel(groupBox_2);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        gridLayout_2->addWidget(label_7, 2, 0, 1, 1);

        comboBox_5 = new QComboBox(groupBox_2);
        comboBox_5->setObjectName(QString::fromUtf8("comboBox_5"));

        gridLayout_2->addWidget(comboBox_5, 2, 2, 1, 1);

        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        gridLayout_2->addWidget(label_8, 3, 0, 1, 1);


        verticalLayout->addWidget(groupBox_2);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        buttonBox = new QDialogButtonBox(PhoneContactProperty);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(PhoneContactProperty);
        QObject::connect(buttonBox, SIGNAL(accepted()), PhoneContactProperty, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), PhoneContactProperty, SLOT(reject()));

        QMetaObject::connectSlotsByName(PhoneContactProperty);
    } // setupUi

    void retranslateUi(QDialog *PhoneContactProperty)
    {
        PhoneContactProperty->setWindowTitle(QApplication::translate("PhoneContactProperty", "Contact Property", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("PhoneContactProperty", "GroupBox", 0, QApplication::UnicodeUTF8));
        label_4->setText(QString());
        label->setText(QApplication::translate("PhoneContactProperty", "\350\201\224\347\263\273\344\272\272\345\247\223\345\220\215:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("PhoneContactProperty", "\350\201\224\347\263\273\344\272\272\347\273\204\357\274\232", 0, QApplication::UnicodeUTF8));
        comboBox_3->clear();
        comboBox_3->insertItems(0, QStringList()
         << QApplication::translate("PhoneContactProperty", "Family", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PhoneContactProperty", "Friends", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("PhoneContactProperty", "Others", 0, QApplication::UnicodeUTF8)
        );
        label_2->setText(QApplication::translate("PhoneContactProperty", "TextLabel", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("PhoneContactProperty", "GroupBox", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("PhoneContactProperty", "\347\224\265\350\257\235\345\217\267\347\240\201\357\274\232", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("PhoneContactProperty", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("PhoneContactProperty", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("PhoneContactProperty", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("PhoneContactProperty", "TextLabel", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("PhoneContactProperty", "TextLabel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PhoneContactProperty: public Ui_PhoneContactProperty {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PHONECONTACTPROPERTY_H
