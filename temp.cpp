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

    // Устанавливает режим по строке (ожидается полное совпадение со значениями "dt", "udt" или "mudt")
    void setMode(const QString &modeStr);

    // Функция, возвращающая текущее значение даты в числовом формате,
    // соответствующем выбранному режиму (dt, udt или mudt)
    QVariant getNumericValue();

    // Функция, устанавливающая значение даты в виджете, принимая входное числовое значение
    // и используя соответствующий режим для преобразования
    void setNumericValue(const QVariant &value);

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
    QLineEdit *lineEdit;       // Обычный QLineEdit (виден при отсутствии режима даты)
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

    // Создаем QFrame для обрамления содержимого
    frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);

    // Вложенная компоновка для содержимого внутри фрейма
    QVBoxLayout *frameLayout = new QVBoxLayout(frame);

    // Метка для отображения текущего режима
    modeLabel = new QLabel("Текущий режим: не установлен", frame);
    frameLayout->addWidget(modeLabel);

    // Обычный QLineEdit (отображается, если режим не определён)
    lineEdit = new QLineEdit(frame);
    frameLayout->addWidget(lineEdit);

    // Виджет выбора даты с форматом, включающим секунды
    dateTimeEdit = new QDateTimeEdit(frame);
    dateTimeEdit->setCalendarPopup(true);
    dateTimeEdit->setDisplayFormat("dd.MM.yyyy HH:mm:ss");
    frameLayout->addWidget(dateTimeEdit);
    dateTimeEdit->hide(); // скрываем по умолчанию

    // Добавляем фрейм во внешнюю компоновку
    mainLayout->addWidget(frame);
    setLayout(mainLayout);
}

void CustomDateWidget::setMode(const QString &modeStr)
{
    // Проверяем, равна ли входная строка одному из требуемых значений
    if (modeStr.compare("dt", Qt::CaseInsensitive) == 0)
    {
        currentMode = DtMode;
        modeLabel->setText("Текущий режим: dt (секунд с 01.01.2000)");
    }
    else if (modeStr.compare("udt", Qt::CaseInsensitive) == 0)
    {
        currentMode = UdtMode;
        modeLabel->setText("Текущий режим: udt (юлианская дата)");
    }
    else if (modeStr.compare("mudt", Qt::CaseInsensitive) == 0)
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
    // При выборе режима для работы с датой скрываем lineEdit и показываем dateTimeEdit
    if (currentMode == DtMode || currentMode == UdtMode || currentMode == MudtMode)
    {
        lineEdit->hide();
        dateTimeEdit->show();
    }
    else // иначе – отображаем lineEdit и скрываем dateTimeEdit
    {
        dateTimeEdit->hide();
        lineEdit->show();
    }
}

// Функция, возвращающая текущее значение даты в числовом формате.
// Используется текущее значение dateTimeEdit и функция convertDateValue.
QVariant CustomDateWidget::getNumericValue()
{
    return convertDateValue(dateTimeEdit->dateTime());
}

// Функция, устанавливающая значение даты в виджете на основе входного числового значения.
// Используем функцию convertToQDateTime для преобразования входного значения.
void CustomDateWidget::setNumericValue(const QVariant &value)
{
    QDateTime dt = convertToQDateTime(value);
    if (dt.isValid())
    {
        dateTimeEdit->setDateTime(dt);
    }
}

// Преобразование: dt (секунды от 01.01.2000) -> QDateTime
QDateTime CustomDateWidget::dtToQDateTime(qint64 seconds)
{
    QDateTime ref(QDate(2000, 1, 1), QTime(0, 0, 0));
    return ref.addSecs(seconds);
}

// Преобразование: QDateTime -> dt (секунды от 01.01.2000)
qint64 CustomDateWidget::QDateTimeToDt(const QDateTime &dt)
{
    QDateTime ref(QDate(2000, 1, 1), QTime(0, 0, 0));
    return ref.secsTo(dt);
}

// Преобразование: QDateTime -> юлианская дата (double)
double CustomDateWidget::qDateTimeToJulian(const QDateTime &dt)
{
    QDateTime utc = dt.toUTC();
    double jd = (utc.toSecsSinceEpoch() / 86400.0) + 2440587.5;
    return jd;
}

// Преобразование: юлианская дата (double) -> QDateTime
QDateTime CustomDateWidget::julianToQDateTime(double jd)
{
    double secs = (jd - 2440587.5) * 86400.0;
    return QDateTime::fromSecsSinceEpoch(static_cast<qint64>(qRound64(secs)), Qt::UTC);
}

// Преобразование: QDateTime -> модифицированная юлианская дата (double)
double CustomDateWidget::qDateTimeToModifiedJulian(const QDateTime &dt)
{
    double jd = qDateTimeToJulian(dt);
    return jd - 2400000.5;
}

// Преобразование: модифицированная юлианская дата (double) -> QDateTime
QDateTime CustomDateWidget::modifiedJulianToQDateTime(double mjd)
{
    double jd = mjd + 2400000.5;
    return julianToQDateTime(jd);
}

// Функция преобразования даты в числовое значение, в зависимости от текущего режима.
QVariant CustomDateWidget::convertDateValue(const QDateTime &dt)
{
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

// Преобразование входного числового значения в QDateTime в зависимости от текущего режима.
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

