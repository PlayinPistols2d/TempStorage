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
   <string>Битовый Конвертер</string>
  </property>
  <widget class="QWidget" name="centralwidget">
   <layout class="QVBoxLayout" name="verticalLayout">
    
    <!-- Ввод 16-битной последовательности -->
    <widget class="QLineEdit" name="binaryInput">
     <property name="placeholderText">
      <string>Введите 16 бит (0 и 1)</string>
     </property>
    </widget>

    <widget class="QLabel" name="labelOr1">
     <property name="text">
      <string>Или</string>
     </property>
     <property name="alignment">
      <set>Qt::AlignCenter</set>
     </property>
    </widget>

    <!-- Ввод двух десятичных чисел -->
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

    <widget class="QLabel" name="labelOr2">
     <property name="text">
      <string>↓ Результат ↓</string>
     </property>
     <property name="alignment">
      <set>Qt::AlignCenter</set>
     </property>
    </widget>

    <!-- Кнопка конвертации -->
    <widget class="QPushButton" name="convertButton">
     <property name="text">
      <string>Конвертировать</string>
     </property>
    </widget>

    <!-- Поле вывода результата -->
    <widget class="QLabel" name="resultLabel">
     <property name="text">
      <string>Здесь будет результат</string>
     </property>
     <property name="alignment">
      <set>Qt::AlignCenter</set>
     </property>
    </widget>

    <!-- Кнопка копирования -->
    <widget class="QPushButton" name="copyButton">
     <property name="text">
      <string>Скопировать</string>
     </property>
    </widget>

   </layout>
  </widget>
  <widget class="QMenuBar" name="menubar"/>
  <widget class="QStatusBar" name="statusbar"/>
 </widget>
 <resources/>
 <connections/>
</ui>




#ifndef BITCONVERTER_H
#define BITCONVERTER_H

#include <QString>

class BitConverter {
public:
    static QString convertToBinary(int type, int kind);
    static QString convertToHex(int type, int kind);
    static bool convertFromBinary(const QString &binary, int &type, int &kind);
    static bool convertFromHex(const QString &hex, int &type, int &kind);
};

#endif // BITCONVERTER_H




#include "BitConverter.h"
#include <QRegularExpression>

QString BitConverter::convertToBinary(int type, int kind) {
    if (type < 0 || type > 1023 || kind < 0 || kind > 63) return "Ошибка";

    int combined = (type << 6) | kind;
    return QString("%1").arg(combined, 16, 2, QChar('0'));  // 16-битная строка
}

QString BitConverter::convertToHex(int type, int kind) {
    if (type < 0 || type > 1023 || kind < 0 || kind > 63) return "Ошибка";

    int combined = (type << 6) | kind;
    return QString("%1").arg(combined, 4, 16, QChar('0')).toUpper();  // HEX (4 символа)
}

bool BitConverter::convertFromBinary(const QString &binary, int &type, int &kind) {
    if (binary.length() != 16 || !QRegularExpression("^[01]{16}$").match(binary).hasMatch()) return false;

    bool ok;
    int value = binary.toInt(&ok, 2);
    if (!ok) return false;

    type = (value >> 6) & 0x3FF;  // 10 бит
    kind = value & 0x3F;          // 6 бит
    return true;
}

bool BitConverter::convertFromHex(const QString &hex, int &type, int &kind) {
    if (hex.length() != 4 || !QRegularExpression("^[0-9A-Fa-f]{4}$").match(hex).hasMatch()) return false;

    bool ok;
    int value = hex.toInt(&ok, 16);
    if (!ok) return false;

    type = (value >> 6) & 0x3FF;
    kind = value & 0x3F;
    return true;
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
    QString binaryInput = ui->binaryInput->text().trimmed();
    QString typeInput = ui->typeInput->text().trimmed();
    QString kindInput = ui->kindInput->text().trimmed();

    QString result;

    if (!binaryInput.isEmpty()) {  // Если введена 16-битная строка
        int type, kind;
        if (BitConverter::convertFromBinary(binaryInput, type, kind)) {
            result = QString("Тип: %1, Вид: %2").arg(type).arg(kind);
        } else {
            result = "Ошибка ввода! Ожидается 16 бит (0 и 1).";
        }
    } 
    else if (!typeInput.isEmpty() && !kindInput.isEmpty()) {  // Если введены два числа
        bool ok1, ok2;
        int type = typeInput.toInt(&ok1);
        int kind = kindInput.toInt(&ok2);

        if (ok1 && ok2 && type >= 0 && type <= 1023 && kind >= 0 && kind <= 63) {
            result = QString("HEX: %1, Биты: %2")
                     .arg(BitConverter::convertToHex(type, kind))
                     .arg(BitConverter::convertToBinary(type, kind));
        } else {
            result = "Ошибка! Числа должны быть в пределах: Тип (0-1023), Вид (0-63).";
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
   <string>Битовый Конвертер</string>
  </property>
  <widget class="QWidget" name="centralwidget">
   <layout class="QVBoxLayout" name="verticalLayout">

    <widget class="QLineEdit" name="binaryInput">
     <property name="placeholderText">
      <string>Введите 16 бит (0 и 1)</string>
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
      <string>Здесь будет результат</string>
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

