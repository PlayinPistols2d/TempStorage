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

    // Создаём QFrame для визуального обрамления содержимого
    frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);

    // Вложенная компоновка для содержимого внутри фрейма
    QVBoxLayout *frameLayout = new QVBoxLayout(frame);

    // Метка для отображения текущего режима
    modeLabel = new QLabel("Текущий режим: не установлен", frame);
    frameLayout->addWidget(modeLabel);

    // Обычный QLineEdit (будет виден, если режим не определён)
    lineEdit = new QLineEdit(frame);
    frameLayout->addWidget(lineEdit);

    // Виджет выбора даты с форматом, включающим секунды
    dateTimeEdit = new QDateTimeEdit(frame);
    dateTimeEdit->setCalendarPopup(true);
    dateTimeEdit->setDisplayFormat("dd.MM.yyyy HH:mm:ss");
    frameLayout->addWidget(dateTimeEdit);
    dateTimeEdit->hide(); // скрываем по умолчанию

    mainLayout->addWidget(frame);
    setLayout(mainLayout);
}

void CustomDateWidget::setMode(const QString &modeStr)
{
    // Сравнение всей строки с ожидаемыми значениями
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
    // Если режим один из режимов для работы с датой – показываем dateTimeEdit, скрывая lineEdit
    if (currentMode == DtMode || currentMode == UdtMode || currentMode == MudtMode)
    {
        lineEdit->hide();
        dateTimeEdit->show();
    }
    else  // в противном случае отображаем lineEdit и скрываем dateTimeEdit
    {
        dateTimeEdit->hide();
        lineEdit->show();
    }
}

// Функция, возвращающая текущее значение даты в числовом формате в виде строки.
QString CustomDateWidget::getNumericValue()
{
    QVariant numericValue = convertDateValue(dateTimeEdit->dateTime());
    if (currentMode == DtMode)
    {
        return QString::number(numericValue.toLongLong());
    }
    else if (currentMode == UdtMode || currentMode == MudtMode)
    {
        return QString::number(numericValue.toDouble(), 'f', 8);
    }
    return QString();
}

// Функция, устанавливающая значение даты в виджете по входной строке с числовым значением.
// Если выбран режим dt, то входная строка интерпретируется как число секунд от 01.01.2000.
void CustomDateWidget::setNumericValue(const QString &value)
{
    QDateTime dt;
    bool ok = false;

    if (currentMode == DtMode)
    {
        // Парсинг строки как целое число – секунд от 01.01.2000.
        qint64 seconds = value.toLongLong(&ok);
        if (ok)
        {
            dt = dtToQDateTime(seconds);
        }
    }
    else if (currentMode == UdtMode)
    {
        double jd = value.toDouble(&ok);
        if (ok)
        {
            dt = julianToQDateTime(jd);
        }
    }
    else if (currentMode == MudtMode)
    {
        double mjd = value.toDouble(&ok);
        if (ok)
        {
            dt = modifiedJulianToQDateTime(mjd);
        }
    }

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
    // Преобразуем в UTC, затем работаем с миллисекундами, добавляя смещение локального времени
    qint64 msUtc = dt.toMSecsSinceEpoch();
    qint64 offsetMs = dt.offsetFromUtc() * 1000LL;
    double localMs = msUtc + offsetMs;
    double jd = (localMs / 86400000.0) + 2440587.5;
    return jd;
}

// Преобразование: юлианская дата (double) -> QDateTime
QDateTime CustomDateWidget::julianToQDateTime(double jd)
{
    qint64 localMs = static_cast<qint64>(qRound64((jd - 2440587.5) * 86400000.0));
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(localMs, Qt::UTC);
    return dt.toLocalTime();
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

// Преобразование даты в числовое значение в зависимости от текущего режима.
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