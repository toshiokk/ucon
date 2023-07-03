/********************************************************************************
** Form generated from reading UI file 'imjqttest.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMJQTTEST_H
#define UI_IMJQTTEST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_imjqttest
{
public:
    QWidget *widget;
    QPushButton *pushButton07;
    QPushButton *pushButton06;
    QPushButton *pushButton05;
    QPushButton *pushButton03;
    QPushButton *pushButton04;
    QPushButton *pushButton01;
    QPushButton *pushButton02;
    QLabel *textLabel01;
    QTextEdit *textEdit1;
    QPushButton *pushButton09;

    void setupUi(QMainWindow *imjqttest)
    {
        if (imjqttest->objectName().isEmpty())
            imjqttest->setObjectName(QString::fromUtf8("imjqttest"));
        imjqttest->resize(357, 169);
        QFont font;
        font.setFamily(QString::fromUtf8("unifont [qt]"));
        font.setPointSize(16);
        imjqttest->setFont(font);
        widget = new QWidget(imjqttest);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(0, 0, 357, 169));
        pushButton07 = new QPushButton(widget);
        pushButton07->setObjectName(QString::fromUtf8("pushButton07"));
        pushButton07->setGeometry(QRect(168, 120, 80, 25));
        pushButton06 = new QPushButton(widget);
        pushButton06->setObjectName(QString::fromUtf8("pushButton06"));
        pushButton06->setGeometry(QRect(88, 120, 80, 25));
        pushButton05 = new QPushButton(widget);
        pushButton05->setObjectName(QString::fromUtf8("pushButton05"));
        pushButton05->setGeometry(QRect(8, 120, 80, 25));
        pushButton03 = new QPushButton(widget);
        pushButton03->setObjectName(QString::fromUtf8("pushButton03"));
        pushButton03->setGeometry(QRect(8, 64, 80, 25));
        pushButton04 = new QPushButton(widget);
        pushButton04->setObjectName(QString::fromUtf8("pushButton04"));
        pushButton04->setGeometry(QRect(8, 92, 80, 25));
        pushButton01 = new QPushButton(widget);
        pushButton01->setObjectName(QString::fromUtf8("pushButton01"));
        pushButton01->setGeometry(QRect(8, 8, 80, 25));
        pushButton02 = new QPushButton(widget);
        pushButton02->setObjectName(QString::fromUtf8("pushButton02"));
        pushButton02->setGeometry(QRect(8, 36, 80, 25));
        textLabel01 = new QLabel(widget);
        textLabel01->setObjectName(QString::fromUtf8("textLabel01"));
        textLabel01->setGeometry(QRect(92, 8, 256, 24));
        QFont font1;
        font1.setFamily(QString::fromUtf8("helvetica [qt]"));
        font1.setPointSize(14);
        textLabel01->setFont(font1);
        textLabel01->setFrameShape(QFrame::StyledPanel);
        textLabel01->setFrameShadow(QFrame::Sunken);
        textLabel01->setWordWrap(false);
        textEdit1 = new QTextEdit(widget);
        textEdit1->setObjectName(QString::fromUtf8("textEdit1"));
        textEdit1->setGeometry(QRect(92, 36, 256, 80));
        pushButton09 = new QPushButton(widget);
        pushButton09->setObjectName(QString::fromUtf8("pushButton09"));
        pushButton09->setGeometry(QRect(248, 120, 100, 25));
        imjqttest->setCentralWidget(widget);
        QWidget::setTabOrder(pushButton01, pushButton02);
        QWidget::setTabOrder(pushButton02, pushButton03);
        QWidget::setTabOrder(pushButton03, pushButton04);
        QWidget::setTabOrder(pushButton04, pushButton05);
        QWidget::setTabOrder(pushButton05, pushButton06);
        QWidget::setTabOrder(pushButton06, pushButton07);
        QWidget::setTabOrder(pushButton07, pushButton09);
        QWidget::setTabOrder(pushButton09, textEdit1);

        retranslateUi(imjqttest);

        QMetaObject::connectSlotsByName(imjqttest);
    } // setupUi

    void retranslateUi(QMainWindow *imjqttest)
    {
        imjqttest->setWindowTitle(QApplication::translate("imjqttest", "IM Qt test", 0, QApplication::UnicodeUTF8));
        pushButton07->setText(QApplication::translate("imjqttest", "test-3", 0, QApplication::UnicodeUTF8));
        pushButton06->setText(QApplication::translate("imjqttest", "test-2", 0, QApplication::UnicodeUTF8));
        pushButton05->setText(QApplication::translate("imjqttest", "test-1", 0, QApplication::UnicodeUTF8));
        pushButton03->setText(QApplication::translate("imjqttest", "Show keypad", 0, QApplication::UnicodeUTF8));
        pushButton04->setText(QApplication::translate("imjqttest", "Hide keypad", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButton01->setToolTip(QApplication::translate("imjqttest", "CALIBRATE", "start calibration", QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        pushButton01->setWhatsThis(QApplication::translate("imjqttest", "Button for calibration", "To start calibration, push this button.", QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        pushButton01->setText(QApplication::translate("imjqttest", "IM ON", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushButton02->setToolTip(QApplication::translate("imjqttest", "RESTART", "Restart touch screen", QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        pushButton02->setWhatsThis(QApplication::translate("imjqttest", "Button for restarting touch screen", "To restart touch screen, push this.", QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        pushButton02->setText(QApplication::translate("imjqttest", "IM OFF", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        textLabel01->setToolTip(QApplication::translate("imjqttest", "TEXTLABEL", "Static text label", QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        textLabel01->setWhatsThis(QApplication::translate("imjqttest", "Text label shows messages", "Text label is used to show the messages", QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        textLabel01->setText(QString());
        pushButton09->setText(QApplication::translate("imjqttest", "Exit", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class imjqttest: public Ui_imjqttest {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMJQTTEST_H
