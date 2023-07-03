/********************************************************************************
** Form generated from reading UI file 'imjqt.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMJQT_H
#define UI_IMJQT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_imjqt
{
public:
    QPushButton *pushButtonConvert;
    QPushButton *pushButtonHiragana;
    QPushButton *pushButtonKatakana;
    QPushButton *pushButtonHankakuKatakana;
    QPushButton *pushButtonZenkakuAlphaNumeric;
    QPushButton *pushButtonHankakuAlphaNumeric;
    QPushButton *pushButtonLeft;
    QPushButton *pushButtonLight;
    QPushButton *pushButtonCommitOne;
    QPushButton *pushButtonCommitAll;
    QPushButton *pushButtonClose;
    QListWidget *listBox1;
    QLabel *textLabel1;
    QLineEdit *lineEdit1;
    QPushButton *pushButtonOn;
    QPushButton *pushButtonOff;
    QLabel *textLabel2;

    void setupUi(QDialog *imjqt)
    {
        if (imjqt->objectName().isEmpty())
            imjqt->setObjectName(QString::fromUtf8("imjqt"));
        imjqt->resize(329, 152);
        QFont font;
        font.setFamily(QString::fromUtf8("ucon12"));
        font.setPointSize(12);
        imjqt->setFont(font);
        pushButtonConvert = new QPushButton(imjqt);
        pushButtonConvert->setObjectName(QString::fromUtf8("pushButtonConvert"));
        pushButtonConvert->setGeometry(QRect(4, 28, 80, 20));
        pushButtonConvert->setAutoDefault(false);
        pushButtonHiragana = new QPushButton(imjqt);
        pushButtonHiragana->setObjectName(QString::fromUtf8("pushButtonHiragana"));
        pushButtonHiragana->setGeometry(QRect(4, 48, 80, 20));
        pushButtonHiragana->setAutoDefault(false);
        pushButtonKatakana = new QPushButton(imjqt);
        pushButtonKatakana->setObjectName(QString::fromUtf8("pushButtonKatakana"));
        pushButtonKatakana->setGeometry(QRect(4, 68, 80, 20));
        pushButtonKatakana->setAutoDefault(false);
        pushButtonHankakuKatakana = new QPushButton(imjqt);
        pushButtonHankakuKatakana->setObjectName(QString::fromUtf8("pushButtonHankakuKatakana"));
        pushButtonHankakuKatakana->setGeometry(QRect(4, 88, 80, 20));
        pushButtonHankakuKatakana->setAutoDefault(false);
        pushButtonZenkakuAlphaNumeric = new QPushButton(imjqt);
        pushButtonZenkakuAlphaNumeric->setObjectName(QString::fromUtf8("pushButtonZenkakuAlphaNumeric"));
        pushButtonZenkakuAlphaNumeric->setGeometry(QRect(4, 108, 80, 20));
        pushButtonZenkakuAlphaNumeric->setAutoDefault(false);
        pushButtonHankakuAlphaNumeric = new QPushButton(imjqt);
        pushButtonHankakuAlphaNumeric->setObjectName(QString::fromUtf8("pushButtonHankakuAlphaNumeric"));
        pushButtonHankakuAlphaNumeric->setGeometry(QRect(4, 128, 80, 20));
        pushButtonHankakuAlphaNumeric->setAutoDefault(false);
        pushButtonLeft = new QPushButton(imjqt);
        pushButtonLeft->setObjectName(QString::fromUtf8("pushButtonLeft"));
        pushButtonLeft->setGeometry(QRect(84, 68, 40, 20));
        pushButtonLeft->setAutoDefault(false);
        pushButtonLight = new QPushButton(imjqt);
        pushButtonLight->setObjectName(QString::fromUtf8("pushButtonLight"));
        pushButtonLight->setGeometry(QRect(124, 68, 40, 20));
        pushButtonLight->setAutoDefault(false);
        pushButtonCommitOne = new QPushButton(imjqt);
        pushButtonCommitOne->setObjectName(QString::fromUtf8("pushButtonCommitOne"));
        pushButtonCommitOne->setGeometry(QRect(84, 88, 80, 20));
        pushButtonCommitOne->setAutoDefault(false);
        pushButtonCommitAll = new QPushButton(imjqt);
        pushButtonCommitAll->setObjectName(QString::fromUtf8("pushButtonCommitAll"));
        pushButtonCommitAll->setGeometry(QRect(84, 108, 80, 20));
        pushButtonCommitAll->setAutoDefault(false);
        pushButtonClose = new QPushButton(imjqt);
        pushButtonClose->setObjectName(QString::fromUtf8("pushButtonClose"));
        pushButtonClose->setGeometry(QRect(84, 128, 80, 20));
        pushButtonClose->setAutoDefault(false);
        listBox1 = new QListWidget(imjqt);
        new QListWidgetItem(listBox1);
        listBox1->setObjectName(QString::fromUtf8("listBox1"));
        listBox1->setGeometry(QRect(164, 28, 160, 80));
        textLabel1 = new QLabel(imjqt);
        textLabel1->setObjectName(QString::fromUtf8("textLabel1"));
        textLabel1->setGeometry(QRect(164, 108, 160, 20));
        textLabel1->setFrameShape(QFrame::Panel);
        textLabel1->setFrameShadow(QFrame::Sunken);
        textLabel1->setWordWrap(false);
        lineEdit1 = new QLineEdit(imjqt);
        lineEdit1->setObjectName(QString::fromUtf8("lineEdit1"));
        lineEdit1->setGeometry(QRect(4, 4, 320, 21));
        pushButtonOn = new QPushButton(imjqt);
        pushButtonOn->setObjectName(QString::fromUtf8("pushButtonOn"));
        pushButtonOn->setGeometry(QRect(84, 28, 80, 20));
        pushButtonOn->setAutoDefault(false);
        pushButtonOff = new QPushButton(imjqt);
        pushButtonOff->setObjectName(QString::fromUtf8("pushButtonOff"));
        pushButtonOff->setGeometry(QRect(84, 48, 80, 20));
        pushButtonOff->setAutoDefault(false);
        textLabel2 = new QLabel(imjqt);
        textLabel2->setObjectName(QString::fromUtf8("textLabel2"));
        textLabel2->setGeometry(QRect(164, 128, 160, 20));
        textLabel2->setFrameShape(QFrame::Panel);
        textLabel2->setFrameShadow(QFrame::Sunken);
        textLabel2->setWordWrap(false);
        QWidget::setTabOrder(lineEdit1, pushButtonConvert);
        QWidget::setTabOrder(pushButtonConvert, pushButtonHiragana);
        QWidget::setTabOrder(pushButtonHiragana, pushButtonKatakana);
        QWidget::setTabOrder(pushButtonKatakana, pushButtonHankakuKatakana);
        QWidget::setTabOrder(pushButtonHankakuKatakana, pushButtonZenkakuAlphaNumeric);
        QWidget::setTabOrder(pushButtonZenkakuAlphaNumeric, pushButtonHankakuAlphaNumeric);
        QWidget::setTabOrder(pushButtonHankakuAlphaNumeric, pushButtonOn);
        QWidget::setTabOrder(pushButtonOn, pushButtonOff);
        QWidget::setTabOrder(pushButtonOff, pushButtonLeft);
        QWidget::setTabOrder(pushButtonLeft, pushButtonLight);
        QWidget::setTabOrder(pushButtonLight, pushButtonCommitOne);
        QWidget::setTabOrder(pushButtonCommitOne, pushButtonCommitAll);
        QWidget::setTabOrder(pushButtonCommitAll, pushButtonClose);
        QWidget::setTabOrder(pushButtonClose, listBox1);

        retranslateUi(imjqt);

        QMetaObject::connectSlotsByName(imjqt);
    } // setupUi

    void retranslateUi(QDialog *imjqt)
    {
        imjqt->setWindowTitle(QApplication::translate("imjqt", "Japanese Input Method", 0, QApplication::UnicodeUTF8));
        pushButtonConvert->setText(QApplication::translate("imjqt", "Convert", 0, QApplication::UnicodeUTF8));
        pushButtonHiragana->setText(QApplication::translate("imjqt", "Hiragana", 0, QApplication::UnicodeUTF8));
        pushButtonKatakana->setText(QApplication::translate("imjqt", "Katakana", 0, QApplication::UnicodeUTF8));
        pushButtonHankakuKatakana->setText(QApplication::translate("imjqt", "Hankaku-Kata", 0, QApplication::UnicodeUTF8));
        pushButtonZenkakuAlphaNumeric->setText(QApplication::translate("imjqt", "Alpha-Numeric", 0, QApplication::UnicodeUTF8));
        pushButtonHankakuAlphaNumeric->setText(QApplication::translate("imjqt", "Hankaku-A-N", 0, QApplication::UnicodeUTF8));
        pushButtonLeft->setText(QApplication::translate("imjqt", "<<", 0, QApplication::UnicodeUTF8));
        pushButtonLight->setText(QApplication::translate("imjqt", ">>", 0, QApplication::UnicodeUTF8));
        pushButtonCommitOne->setText(QApplication::translate("imjqt", "Commit-One", 0, QApplication::UnicodeUTF8));
        pushButtonCommitAll->setText(QApplication::translate("imjqt", "Commit-All", 0, QApplication::UnicodeUTF8));
        pushButtonClose->setText(QApplication::translate("imjqt", "CLOSE", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = listBox1->isSortingEnabled();
        listBox1->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = listBox1->item(0);
        ___qlistwidgetitem->setText(QApplication::translate("imjqt", "New Item", 0, QApplication::UnicodeUTF8));
        listBox1->setSortingEnabled(__sortingEnabled);

        textLabel1->setText(QApplication::translate("imjqt", "123456789012345678", 0, QApplication::UnicodeUTF8));
        pushButtonOn->setText(QApplication::translate("imjqt", "ON", 0, QApplication::UnicodeUTF8));
        pushButtonOff->setText(QApplication::translate("imjqt", "OFF", 0, QApplication::UnicodeUTF8));
        textLabel2->setText(QApplication::translate("imjqt", "123456789012345678", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class imjqt: public Ui_imjqt {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMJQT_H
