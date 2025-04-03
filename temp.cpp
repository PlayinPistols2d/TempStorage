#ifndef BITCONVERTER_H
#define BITCONVERTER_H

#include <QString>

class BitConverter {
public:
    static std::pair<int, int> hexToTypeKind(const QString& hex);
    static QString typeKindToHex(int type, int kind);
};

#endif // BITCONVERTER_H



#include "BitConverter.h"

std::pair<int, int> BitConverter::hexToTypeKind(const QString& hex) {
    bool ok;
    int value = hex.toInt(&ok, 16);
    if (!ok) return {-1, -1};  // Ошибка: некорректный ввод

    int type = (value >> 6) & 0x3FF; // 10 старших бит
    int kind = value & 0x3F;         // 6 младших бит

    return {type, kind};
}

QString BitConverter::typeKindToHex(int type, int kind) {
    if (type < 0 || type > 1023 || kind < 0 || kind > 63)
        return "Ошибка: Выход за пределы";

    int value = (type << 6) | kind;
    return QString::number(value, 16).toUpper().rightJustified(4, '0');
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
    void on_convertButton_clicked();
    void on_copyButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H



#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "BitConverter.h"
#include <QClipboard>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_convertButton_clicked() {
    if (ui->hexInput->text().isEmpty()) {  // Если вводится два числа
        bool ok1, ok2;
        int type = ui->typeInput->text().toInt(&ok1, 2); // Двоичный ввод
        int kind = ui->kindInput->text().toInt(&ok2, 2);

        if (!ok1 || !ok2 || type > 1023 || kind > 63) {
            QMessageBox::warning(this, "Ошибка", "Некорректный ввод чисел");
            return;
        }

        QString hexResult = BitConverter::typeKindToHex(type, kind);
        ui->resultLabel->setText("HEX: " + hexResult);
    } else {  // Если вводится HEX
        auto [type, kind] = BitConverter::hexToTypeKind(ui->hexInput->text());

        if (type == -1) {
            QMessageBox::warning(this, "Ошибка", "Некорректный HEX-ввод");
            return;
        }

        ui->resultLabel->setText("Тип: " + QString::number(type, 2) + " | Вид: " + QString::number(kind, 2));
    }
}

void MainWindow::on_copyButton_clicked() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->resultLabel->text());
    QMessageBox::information(this, "Скопировано", "Результат скопирован в буфер обмена");
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
  <widget class="QWidget" name="centralwidget">
   <widget class="QLineEdit" name="hexInput">
    <property name="placeholderText">
     <string>Введите HEX (4 знака)</string>
    </property>
   </widget>
   <widget class="QLineEdit" name="typeInput">
    <property name="placeholderText">
     <string>Введите тип (10 бит)</string>
    </property>
   </widget>
   <widget class="QLineEdit" name="kindInput">
    <property name="placeholderText">
     <string>Введите вид (6 бит)</string>
    </property>
   </widget>
   <widget class="QPushButton" name="convertButton">
    <property name="text">
     <string>Конвертировать</string>
    </property>
   </widget>
   <widget class="QPushButton" name="copyButton">
    <property name="text">
     <string>Скопировать</string>
    </property>
   </widget>
   <widget class="QLabel" name="resultLabel">
    <property name="text">
     <string>Результат</string>
    </property>
   </widget>
  </widget>
 </widget>
 <resources/>
 <connections/>
</ui>
