/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QLineEdit *lineEdit_3;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *lineEdit_4;
    QLabel *label;
    QTextEdit *textEdit;
    QTextEdit *textEdit_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QLineEdit *lineEdit_5;
    QLabel *label_4;
    QPushButton *pushButton_4;
    QLabel *label_5;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;
    QPushButton *pushButton_10;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(807, 410);
        lineEdit_3 = new QLineEdit(Widget);
        lineEdit_3->setObjectName("lineEdit_3");
        lineEdit_3->setGeometry(QRect(80, 220, 111, 27));
        lineEdit_3->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);
        label_2 = new QLabel(Widget);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(100, 190, 69, 19));
        label_3 = new QLabel(Widget);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(250, 190, 69, 19));
        lineEdit_4 = new QLineEdit(Widget);
        lineEdit_4->setObjectName("lineEdit_4");
        lineEdit_4->setGeometry(QRect(230, 220, 111, 27));
        lineEdit_4->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignVCenter);
        label = new QLabel(Widget);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 10, 60, 60));
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setMinimumSize(QSize(60, 60));
        label->setStyleSheet(QString::fromUtf8("border-radius:30px;\n"
"background-color: rgb(255, 0, 4);"));
        label->setAlignment(Qt::AlignmentFlag::AlignCenter);
        textEdit = new QTextEdit(Widget);
        textEdit->setObjectName("textEdit");
        textEdit->setGeometry(QRect(408, 12, 387, 132));
        textEdit_2 = new QTextEdit(Widget);
        textEdit_2->setObjectName("textEdit_2");
        textEdit_2->setGeometry(QRect(409, 152, 256, 28));
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Ignored);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(textEdit_2->sizePolicy().hasHeightForWidth());
        textEdit_2->setSizePolicy(sizePolicy1);
        textEdit_2->setMinimumSize(QSize(0, 10));
        textEdit_2->setSizeIncrement(QSize(0, 23));
        pushButton_3 = new QPushButton(Widget);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setGeometry(QRect(672, 152, 80, 28));
        pushButton_3->setSizeIncrement(QSize(0, 23));
        pushButton = new QPushButton(Widget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(79, 80, 111, 28));
        pushButton_2 = new QPushButton(Widget);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(79, 130, 111, 28));
        lineEdit_5 = new QLineEdit(Widget);
        lineEdit_5->setObjectName("lineEdit_5");
        lineEdit_5->setGeometry(QRect(152, 300, 111, 27));
        lineEdit_5->setAlignment(Qt::AlignmentFlag::AlignCenter);
        label_4 = new QLabel(Widget);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(190, 270, 69, 19));
        pushButton_4 = new QPushButton(Widget);
        pushButton_4->setObjectName("pushButton_4");
        pushButton_4->setGeometry(QRect(420, 360, 101, 28));
        label_5 = new QLabel(Widget);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(570, 200, 221, 141));
        label_5->setStyleSheet(QString::fromUtf8("background-color: rgb(122, 122, 122);"));
        pushButton_5 = new QPushButton(Widget);
        pushButton_5->setObjectName("pushButton_5");
        pushButton_5->setGeometry(QRect(560, 360, 99, 28));
        pushButton_6 = new QPushButton(Widget);
        pushButton_6->setObjectName("pushButton_6");
        pushButton_6->setGeometry(QRect(410, 250, 141, 28));
        pushButton_7 = new QPushButton(Widget);
        pushButton_7->setObjectName("pushButton_7");
        pushButton_7->setGeometry(QRect(700, 360, 99, 28));
        pushButton_8 = new QPushButton(Widget);
        pushButton_8->setObjectName("pushButton_8");
        pushButton_8->setGeometry(QRect(230, 80, 111, 28));
        pushButton_9 = new QPushButton(Widget);
        pushButton_9->setObjectName("pushButton_9");
        pushButton_9->setGeometry(QRect(230, 130, 111, 28));
        pushButton_10 = new QPushButton(Widget);
        pushButton_10->setObjectName("pushButton_10");
        pushButton_10->setGeometry(QRect(280, 360, 101, 28));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        label_2->setText(QCoreApplication::translate("Widget", "\347\216\257\345\242\203\346\270\251\345\272\246", nullptr));
        label_3->setText(QCoreApplication::translate("Widget", "\347\216\257\345\242\203\346\271\277\345\272\246", nullptr));
        label->setText(QString());
        pushButton_3->setText(QCoreApplication::translate("Widget", "\345\217\221\351\200\201", nullptr));
        pushButton->setText(QCoreApplication::translate("Widget", "\350\277\236\346\216\245", nullptr));
        pushButton_2->setText(QCoreApplication::translate("Widget", "\346\226\255\345\274\200", nullptr));
        label_4->setText(QCoreApplication::translate("Widget", "\346\226\271\345\220\221", nullptr));
        pushButton_4->setText(QCoreApplication::translate("Widget", "left", nullptr));
        label_5->setText(QString());
        pushButton_5->setText(QCoreApplication::translate("Widget", "front", nullptr));
        pushButton_6->setText(QCoreApplication::translate("Widget", "\350\216\267\345\217\226\350\256\276\345\244\207\347\224\273\351\235\242", nullptr));
        pushButton_7->setText(QCoreApplication::translate("Widget", "right", nullptr));
        pushButton_8->setText(QCoreApplication::translate("Widget", "\350\266\213\345\205\211\346\250\241\345\274\217", nullptr));
        pushButton_9->setText(QCoreApplication::translate("Widget", "\346\211\213\345\212\250\346\250\241\345\274\217", nullptr));
        pushButton_10->setText(QCoreApplication::translate("Widget", "\346\265\201\346\260\264\347\201\257\350\255\246\347\244\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
