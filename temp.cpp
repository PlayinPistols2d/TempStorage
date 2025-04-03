#ifndef BITCONVERTER_H
#define BITCONVERTER_H

#include <QString>

class BitConverter
{
public:
    static QString hexToBinary(const QString &hex);
    static QString binaryToHex(const QString &typeBin, const QString &kindBin);
};

#endif // BITCONVERTER_H




#include "bitconverter.h"

QString BitConverter::hexToBinary(const QString &hex)
{
    bool ok;
    uint16_t value = hex.toUShort(&ok, 16);

    if (!ok || value > 0xFFFF) {
        return "Ошибка: Некорректный HEX";
    }

    uint16_t type = (value >> 6) & 0x3FF; // Старшие 10 бит
    uint16_t kind = value & 0x3F;        // Младшие 6 бит

    return QString("Тип: %1 (0b%2)\nВид: %3 (0b%4)")
        .arg(type)
        .arg(QString::number(type, 2).rightJustified(10, '0'))
        .arg(kind)
        .arg(QString::number(kind, 2).rightJustified(6, '0'));
}

QString BitConverter::binaryToHex(const QString &typeBin, const QString &kindBin)
{
    bool okType, okKind;
    uint16_t type = typeBin.toUShort(&okType, 2);
    uint16_t kind = kindBin.toUShort(&okKind, 2);

    if (!okType || !okKind || type > 0x3FF || kind > 0x3F) {
        return "Ошибка: Некорректный ввод";
    }

    uint16_t result = (type << 6) | kind;
    return QString("HEX: 0x%1").arg(result, 4, 16, QChar('0')).toUpper();
}





#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QClipboard>
#include "bitconverter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_convertButton_clicked();
    void on_copyButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H




#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->hexInput->setPlaceholderText("Введите HEX (0xFFFF)");
    ui->typeInput->setPlaceholderText("Введите 10-битное число (бинарно)");
    ui->kindInput->setPlaceholderText("Введите 6-битное число (бинарно)");

    connect(ui->convertButton, &QPushButton::clicked, this, &MainWindow::on_convertButton_clicked);
    connect(ui->copyButton, &QPushButton::clicked, this, &MainWindow::on_copyButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_convertButton_clicked()
{
    QString output;
    if (!ui->hexInput->text().isEmpty()) {
        output = BitConverter::hexToBinary(ui->hexInput->text());
    } else if (!ui->typeInput->text().isEmpty() && !ui->kindInput->text().isEmpty()) {
        output = BitConverter::binaryToHex(ui->typeInput->text(), ui->kindInput->text());
    } else {
        output = "Ошибка: Введите данные";
    }
    
    ui->outputLabel->setText(output);
}

void MainWindow::on_copyButton_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->outputLabel->text());
}






<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>MainWindow</class>
 <widget class="QMainWindow" name="MainWindow">
  <property name="geometry">
   <rect>
    <x>0</x>
    <y>0</y>
    <width>400</width>
    <height>300</height>
   </rect>
  </property>
  <property name="windowTitle">
   <string>Bit Converter</string>
  </property>
  <widget class="QWidget" name="centralwidget">
   <widget class="QLineEdit" name="hexInput">
    <property name="geometry">
     <rect>
      <x>20</x>
      <y>20</y>
      <width>360</width>
      <height>30</height>
     </rect>
    </property>
   </widget>
   <widget class="QLineEdit" name="typeInput">
    <property name="geometry">
     <rect>
      <x>20</x>
      <y>60</y>
      <width>175</width>
      <height>30</height>
     </rect>
    </property>
   </widget>
   <widget class="QLineEdit" name="kindInput">
    <property name="geometry">
     <rect>
      <x>205</x>
      <y>60</y>
      <width>175</width>
      <height>30</height>
     </rect>
    </property>
   </widget>
   <widget class="QPushButton" name="convertButton">
    <property name="geometry">
     <rect>
      <x>20</x>
      <y>100</y>
      <width>360</width>
      <height>30</height>
     </rect>
    </property>
    <property name="text">
     <string>Конвертировать</string>
    </property>
   </widget>
   <widget class="QLabel" name="outputLabel">
    <property name="geometry">
     <rect>
      <x>20</x>
      <y>140</y>
      <width>360</width>
      <height>60</height>
     </rect>
    </property>
    <property name="text">
     <string>Результат</string>
    </property>
    <property name="alignment">
     <set>Qt::AlignCenter</set>
    </property>
   </widget>
   <widget class="QPushButton" name="copyButton">
    <property name="geometry">
     <rect>
      <x>20</x>
      <y>210</y>
      <width>360</width>
      <height>30</height>
     </rect>
    </property>
    <property name="text">
     <string>Копировать</string>
    </property>
   </widget>
  </widget>
  <layout class="QVBoxLayout" name="verticalLayout"/>
  <widget class="QMenuBar" name="menubar"/>
  <widget class="QStatusBar" name="statusbar"/>
 </widget>
 <resources/>
 <connections/>
</ui>
