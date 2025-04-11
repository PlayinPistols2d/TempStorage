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
    // Если режим dt – выводим целое число, иначе формат double с 8 знаками после запятой
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
void CustomDateWidget::setNumericValue(const QString &value)
{
    QVariant val;
    if (currentMode == DtMode)
    {
        val = QVariant(value.toLongLong());
    }
    else if (currentMode == UdtMode || currentMode == MudtMode)
    {
        val = QVariant(value.toDouble());
    }
    QDateTime dt = convertToQDateTime(val);
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
// Для вычисления локального юлианского дня учитывается смещение от UTC.
double CustomDateWidget::qDateTimeToJulian(const QDateTime &dt)
{
    // Получаем количество миллисекунд от Unix-эпохи (1970-01-01 00:00:00 UTC)
    qint64 msUtc = dt.toMSecsSinceEpoch();
    // Учитываем локальное смещение в миллисекундах (offsetFromUtc возвращает разницу в секундах)
    qint64 offsetMs = dt.offsetFromUtc() * 1000LL;
    // Локальное количество миллисекунд: фактически время, которое отображается локально
    double localMs = msUtc + offsetMs;
    // Вычисляем юлианскую дату: количество дней с 1970-01-01 плюс константа,
    // соответствующая юлианской дате Unix-эпохи (2440587.5)
    double jd = (localMs / 86400000.0) + 2440587.5;
    return jd;
}

// Преобразование: юлианская дата (double) -> QDateTime
// Обратное преобразование выполняется так, чтобы полученный QDateTime был в локальном времени.
QDateTime CustomDateWidget::julianToQDateTime(double jd)
{
    // Вычисляем локальное время в миллисекундах: (jd - 2440587.5) * msec в сутках
    qint64 localMs = static_cast<qint64>(qRound64((jd - 2440587.5) * 86400000.0));
    // Получаем QDateTime в UTC, затем переводим в локальное время
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(localMs, Qt::UTC);
    return dt.toLocalTime();
}

// Преобразование: QDateTime -> модифицированная юлианская дата (double)
// MJD = JD - 2400000.5
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