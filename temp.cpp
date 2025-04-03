#ifndef BITCONVERTER_H
#define BITCONVERTER_H

#include <QString>

class BitConverter {
public:
    static QString hexToDecimalPair(const QString &hex);
    static QString decimalPairToHex(int type, int kind);
};

#endif // BITCONVERTER_H


#include "BitConverter.h"
#include <QRegularExpression>

QString BitConverter::hexToDecimalPair(const QString &hex) {
    if (hex.length() != 4 || !QRegularExpression("^[0-9A-Fa-f]{4}$").match(hex).hasMatch())
        return "Ошибка! Введите 4-значный HEX.";

    bool ok;
    int value = hex.toInt(&ok, 16);
    if (!ok) return "Ошибка конвертации!";

    int type = (value >> 6) & 0x3FF; // 10 старших бит
    int kind = value & 0x3F;         // 6 младших бит

    return QString("Тип: %1, Вид: %2").arg(type).arg(kind);
}

QString BitConverter::decimalPairToHex(int type, int kind) {
    if (type < 0 || type > 1023 || kind < 0 || kind > 63)
        return "Ошибка! Тип (0-1023), Вид (0-63).";

    int combined = (type << 6) | kind;
    return QString("%1").arg(combined, 4, 16, QChar('0')).toUpper();
}




#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConvertClicked();
    void onCopyClicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H





#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "BitConverter.h"
#include <QClipboard>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->convertButton, &QPushButton::clicked, this, &MainWindow::onConvertClicked);
    connect(ui->copyButton, &QPushButton::clicked, this, &MainWindow::onCopyClicked);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onConvertClicked() {
    QString hexInput = ui->hexInput->text().trimmed();
    QString typeInput = ui->typeInput->text().trimmed();
    QString kindInput = ui->kindInput->text().trimmed();
    QString result;

    if (!hexInput.isEmpty()) {  // Конвертация из HEX в два числа
        result = BitConverter::hexToDecimalPair(hexInput);
    } 
    else if (!typeInput.isEmpty() && !kindInput.isEmpty()) {  // Конвертация из двух чисел в HEX
        bool ok1, ok2;
        int type = typeInput.toInt(&ok1);
        int kind = kindInput.toInt(&ok2);

        if (ok1 && ok2) {
            result = "HEX: " + BitConverter::decimalPairToHex(type, kind);
        } else {
            result = "Ошибка! Введите корректные числа.";
        }
    } 
    else {
        result = "Введите данные!";
    }

    ui->resultLabel->setText(result);
}

void MainWindow::onCopyClicked() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->resultLabel->text());
}




<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>MainWindow</class>
 <widget class="QMainWindow" name="MainWindow">
  <property name="geometry">
   <rect>
    <x>100</x>
    <y>100</y>
    <width>400</width>
    <height>300</height>
   </rect>
  </property>
  <property name="windowTitle">
   <string>HEX Конвертер</string>
  </property>
  <widget class="QWidget" name="centralwidget">
   <layout class="QVBoxLayout" name="verticalLayout">

    <widget class="QLineEdit" name="hexInput">
     <property name="placeholderText">
      <string>Введите HEX (4 символа)</string>
     </property>
    </widget>

    <widget class="QLabel" name="labelOr">
     <property name="text">
      <string>Или введите два числа:</string>
     </property>
     <property name="alignment">
      <set>Qt::AlignCenter</set>
     </property>
    </widget>

    <layout class="QHBoxLayout" name="horizontalLayout">
     <widget class="QLineEdit" name="typeInput">
      <property name="placeholderText">
       <string>Тип (0-1023)</string>
      </property>
     </widget>
     <widget class="QLineEdit" name="kindInput">
      <property name="placeholderText">
       <string>Вид (0-63)</string>
      </property>
     </widget>
    </layout>

    <widget class="QPushButton" name="convertButton">
     <property name="text">
      <string>Конвертировать</string>
     </property>
    </widget>

    <widget class="QLabel" name="resultLabel">
     <property name="text">
      <string>Результат появится здесь</string>
     </property>
    </widget>

    <widget class="QPushButton" name="copyButton">
     <property name="text">
      <string>Скопировать</string>
     </property>
    </widget>

   </layout>
  </widget>
 </widget>
</ui>
