#ifndef CUSTOMDATEWIDGET_H
#define CUSTOMDATEWIDGET_H

#include <QWidget>
#include <QVariant>
#include <QDateTime>

class QLineEdit;
class QDateTimeEdit;
class QLabel;
class QFrame;

class CustomDateWidget : public QWidget
{
    Q_OBJECT

public:
    // Режимы представления даты
    enum DateMode {
        DtMode,    // dt: количество секунд с 01.01.2000
        UdtMode,   // udt: юлианская дата (double)
        MudtMode,  // mudt: модифицированная юлианская дата (double)
        NoneMode   // режим не установлен
    };

    explicit CustomDateWidget(QWidget *parent = nullptr);

    // Устанавливает режим по строке (ищем подстроки "dt", "udt" или "mudt")
    void setMode(const QString &modeStr);

    // Функции преобразования даты в числовое значение и наоборот
    QVariant convertDateValue(const QDateTime &dt);
    QDateTime convertToQDateTime(const QVariant &value);

    // Статические функции для конвертации дат

    // Преобразование: dt (секунды с 01.01.2000) -> QDateTime
    static QDateTime dtToQDateTime(qint64 seconds);

    // Преобразование: QDateTime -> dt (секунды с 01.01.2000)
    static qint64 QDateTimeToDt(const QDateTime &dt);

    // Преобразование: QDateTime -> юлианская дата (double)
    static double qDateTimeToJulian(const QDateTime &dt);

    // Преобразование: юлианская дата (double) -> QDateTime
    static QDateTime julianToQDateTime(double jd);

    // Преобразование: QDateTime -> модифицированная юлианская дата (double)
    static double qDateTimeToModifiedJulian(const QDateTime &dt);

    // Преобразование: модифицированная юлианская дата (double) -> QDateTime
    static QDateTime modifiedJulianToQDateTime(double mjd);

private:
    // Обновляет видимость элементов в зависимости от выбранного режима
    void updateWidgetDisplay();

    QLabel *modeLabel;         // Метка отображения текущего режима
    QLineEdit *lineEdit;         // Обычный QLineEdit (виден при отсутствии режима даты)
    QDateTimeEdit *dateTimeEdit; // Виджет выбора даты (виден при установленном режиме даты)
    QFrame *frame;             // Фрейм для визуального обрамления виджета

    DateMode currentMode;      // Текущий режим обработки даты
};

#endif // CUSTOMDATEWIDGET_H



#include "CustomDateWidget.h"

#include <QLineEdit>
#include <QDateTimeEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>
#include <QDateTime>
#include <QDate>
#include <QString>
#include <QtMath>

CustomDateWidget::CustomDateWidget(QWidget *parent)
    : QWidget(parent), currentMode(NoneMode)
{
    // Основная компоновка виджета
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Создаем QFrame для обрамления и настраиваем его внешний вид
    frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);

    // Вложенная компоновка для содержимого внутри рамки
    QVBoxLayout *frameLayout = new QVBoxLayout(frame);

    // Метка для отображения текущего режима
    modeLabel = new QLabel("Текущий режим: не установлен", frame);
    frameLayout->addWidget(modeLabel);

    // Обычный QLineEdit (отображается, если режим не определён)
    lineEdit = new QLineEdit(frame);
    frameLayout->addWidget(lineEdit);

    // Виджет выбора даты, включающий секунды в формате
    dateTimeEdit = new QDateTimeEdit(frame);
    dateTimeEdit->setCalendarPopup(true);
    // Задаем формат, включающий секунды
    dateTimeEdit->setDisplayFormat("dd.MM.yyyy HH:mm:ss");
    frameLayout->addWidget(dateTimeEdit);
    dateTimeEdit->hide(); // скрываем по умолчанию

    // Добавляем фрейм в основную компоновку
    mainLayout->addWidget(frame);
    setLayout(mainLayout);
}

void CustomDateWidget::setMode(const QString &modeStr)
{
    // Определяем режим в зависимости от содержимого строки
    if (modeStr.contains("dt", Qt::CaseInsensitive))
    {
        currentMode = DtMode;
        modeLabel->setText("Текущий режим: dt (секунд с 01.01.2000)");
    }
    else if (modeStr.contains("udt", Qt::CaseInsensitive))
    {
        currentMode = UdtMode;
        modeLabel->setText("Текущий режим: udt (юлианская дата)");
    }
    else if (modeStr.contains("mudt", Qt::CaseInsensitive))
    {
        currentMode = MudtMode;
        modeLabel->setText("Текущий режим: mudt (модифицированная юлианская дата)");
    }
    else
    {
        currentMode = NoneMode;
        modeLabel->setText("Текущий режим: не определён");
    }

    updateWidgetDisplay();
}

void CustomDateWidget::updateWidgetDisplay()
{
    // При выборе одного из режимов даты скрываем lineEdit и показываем dateTimeEdit
    if (currentMode == DtMode || currentMode == UdtMode || currentMode == MudtMode)
    {
        lineEdit->hide();
        dateTimeEdit->show();
    }
    else // иначе отображаем lineEdit и скрываем dateTimeEdit
    {
        dateTimeEdit->hide();
        lineEdit->show();
    }
}

// Статическая функция: преобразует количество секунд (от 01.01.2000)
// в QDateTime.
QDateTime CustomDateWidget::dtToQDateTime(qint64 seconds)
{
    QDateTime ref(QDate(2000, 1, 1), QTime(0, 0, 0));
    return ref.addSecs(seconds);
}

// Статическая функция: возвращает число секунд между dt и 01.01.2000.
qint64 CustomDateWidget::QDateTimeToDt(const QDateTime &dt)
{
    QDateTime ref(QDate(2000, 1, 1), QTime(0, 0, 0));
    return ref.secsTo(dt);
}

// Статическая функция: преобразует QDateTime в юлианскую дату (double).
double CustomDateWidget::qDateTimeToJulian(const QDateTime &dt)
{
    QDateTime utc = dt.toUTC();
    double jd = (utc.toSecsSinceEpoch() / 86400.0) + 2440587.5;
    return jd;
}

// Статическая функция: преобразует юлианскую дату (double) в QDateTime.
QDateTime CustomDateWidget::julianToQDateTime(double jd)
{
    double secs = (jd - 2440587.5) * 86400.0;
    return QDateTime::fromSecsSinceEpoch(static_cast<qint64>(qRound64(secs)), Qt::UTC);
}

// Статическая функция: преобразует QDateTime в модифицированную юлианскую дату (MJD).
double CustomDateWidget::qDateTimeToModifiedJulian(const QDateTime &dt)
{
    double jd = qDateTimeToJulian(dt);
    return jd - 2400000.5;
}

// Статическая функция: преобразует модифицированную юлианскую дату (MJD) в QDateTime.
QDateTime CustomDateWidget::modifiedJulianToQDateTime(double mjd)
{
    double jd = mjd + 2400000.5;
    return julianToQDateTime(jd);
}

QVariant CustomDateWidget::convertDateValue(const QDateTime &dt)
{
    // В зависимости от выбранного режима возвращается числовое представление даты.
    switch (currentMode)
    {
        case DtMode:
            return QVariant::fromValue(QDateTimeToDt(dt));
        case UdtMode:
            return QVariant::fromValue(qDateTimeToJulian(dt));
        case MudtMode:
            return QVariant::fromValue(qDateTimeToModifiedJulian(dt));
        default:
            return QVariant();
    }
}

QDateTime CustomDateWidget::convertToQDateTime(const QVariant &value)
{
    if (!value.isValid())
        return QDateTime();

    switch (currentMode)
    {
        case DtMode:
            return dtToQDateTime(value.toLongLong());
        case UdtMode:
            return julianToQDateTime(value.toDouble());
        case MudtMode:
            return modifiedJulianToQDateTime(value.toDouble());
        default:
            return QDateTime();
    }
}
