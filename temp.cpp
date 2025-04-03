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
    
    <!-- Ввод HEX -->
    <widget class="QLineEdit" name="hexInput">
     <property name="placeholderText">
      <string>Введите HEX (4 знака)</string>
     </property>
    </widget>

    <widget class="QLabel" name="labelOr">
     <property name="text">
      <string>Или введите два числа (в двоичном формате):</string>
     </property>
     <property name="alignment">
      <set>Qt::AlignCenter</set>
     </property>
    </widget>

    <!-- Ввод типа и вида -->
    <layout class="QHBoxLayout" name="horizontalLayout">
     <widget class="QLineEdit" name="typeInput">
      <property name="placeholderText">
       <string>Тип (10 бит)</string>
      </property>
     </widget>
     <widget class="QLineEdit" name="kindInput">
      <property name="placeholderText">
       <string>Вид (6 бит)</string>
      </property>
     </widget>
    </layout>

    <!-- Кнопки -->
    <widget class="QPushButton" name="convertButton">
     <property name="text">
      <string>Конвертировать</string>
     </property>
    </widget>

    <widget class="QLabel" name="resultLabel">
     <property name="text">
      <string>Результат</string>
     </property>
     <property name="alignment">
      <set>Qt::AlignCenter</set>
     </property>
    </widget>

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