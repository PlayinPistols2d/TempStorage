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
#include <QtGlobal> // Для qFuzzyIsNull

CustomDateWidget::CustomDateWidget(QWidget *parent)
    : QWidget(parent), currentMode(NoneMode)
{
    // Основная компоновка
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Создаем фрейм для визуального обрамления
    frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);

    // Вложенная компоновка для содержимого фрейма
    QVBoxLayout *frameLayout = new QVBoxLayout(frame);

    // Метка для отображения режима
    modeLabel = new QLabel("Текущий режим: не установлен", frame);
    frameLayout->addWidget(modeLabel);

    // Обычный QLineEdit (если режим не установлен)
    lineEdit = new QLineEdit(frame);
    frameLayout->addWidget(lineEdit);

    // Виджет выбора даты (с форматом с секундами)
    dateTimeEdit = new QDateTimeEdit(frame);
    dateTimeEdit->setCalendarPopup(true);
    dateTimeEdit->setDisplayFormat("dd.MM.yyyy HH:mm:ss");
    frameLayout->addWidget(dateTimeEdit);
    dateTimeEdit->hide();

    mainLayout->addWidget(frame);
    setLayout(mainLayout);
}

void CustomDateWidget::setMode(const QString &modeStr)
{
    // Полное сравнение строки (без учета регистра)
    if (modeStr.compare("dt", Qt::CaseInsensitive) == 0) {
        currentMode = DtMode;
        modeLabel->setText("Текущий режим: dt (секунд с 01.01.2000)");
    } else if (modeStr.compare("udt", Qt::CaseInsensitive) == 0) {
        currentMode = UdtMode;
        modeLabel->setText("Текущий режим: udt (юлианская дата)");
    } else if (modeStr.compare("mudt", Qt::CaseInsensitive) == 0) {
        currentMode = MudtMode;
        modeLabel->setText("Текущий режим: mudt (модифицированная юлианская дата)");
    } else {
        currentMode = NoneMode;
        modeLabel->setText("Текущий режим: не определён");
    }

    updateWidgetDisplay();
}

void CustomDateWidget::updateWidgetDisplay()
{
    // Если режим даты выбран – показываем QDateTimeEdit, иначе QLineEdit
    if (currentMode == DtMode || currentMode == UdtMode || currentMode == MudtMode) {
        lineEdit->hide();
        dateTimeEdit->show();
    } else {
        dateTimeEdit->hide();
        lineEdit->show();
    }
}

QString CustomDateWidget::getNumericValue()
{
    QVariant numericValue = convertDateValue(dateTimeEdit->dateTime());
    if (currentMode == DtMode) {
        return QString::number(numericValue.toLongLong());
    }
    else if (currentMode == UdtMode || currentMode == MudtMode) {
        return QString::number(numericValue.toDouble(), 'f', 8);
    }
    return QString();
}

void CustomDateWidget::setNumericValue(const QString &value)
{
    QDateTime dt;
    bool ok = false;
    if (currentMode == DtMode) {
        qint64 seconds = value.toLongLong(&ok);
        if (ok)
            dt = dtToQDateTime(seconds);
    } else if (currentMode == UdtMode) {
        double jd = value.toDouble(&ok);
        if (ok)
            dt = julianToQDateTime(jd);
    } else if (currentMode == MudtMode) {
        double mjd = value.toDouble(&ok);
        if (ok)
            dt = modifiedJulianToQDateTime(mjd);
    }
    if (dt.isValid())
        dateTimeEdit->setDateTime(dt);
}

// dt режим: число секунд от 01.01.2000 → QDateTime
QDateTime CustomDateWidget::dtToQDateTime(qint64 seconds)
{
    QDateTime ref(QDate(2000, 1, 1), QTime(0, 0, 0));
    return ref.addSecs(seconds);
}

// QDateTime → число секунд от 01.01.2000
qint64 CustomDateWidget::QDateTimeToDt(const QDateTime &dt)
{
    QDateTime ref(QDate(2000, 1, 1), QTime(0, 0, 0));
    return ref.secsTo(dt);
}

// QDateTime → юлианская дата (double)
// Если dt равен ссылочной дате, возвращаем 0.
double CustomDateWidget::qDateTimeToJulian(const QDateTime &dt)
{
    QDateTime ref = dtToQDateTime(0);
    if (dt == ref)
        return 0.0;
    // Работаем с локальными миллисекундами
    qint64 msUtc = dt.toMSecsSinceEpoch();
    qint64 offsetMs = dt.offsetFromUtc() * 1000LL;
    double localMs = static_cast<double>(msUtc + offsetMs);
    double jd = (localMs / 86400000.0) + 2440587.5;
    return jd;
}

// Юлианская дата (double) → QDateTime
// Если входное значение jd равно 0 (или почти 0), считаем, что это ссылка – 01.01.2000.
QDateTime CustomDateWidget::julianToQDateTime(double jd)
{
    if (qFuzzyIsNull(jd))
        return dtToQDateTime(0);
    qint64 localMs = static_cast<qint64>(qRound64((jd - 2440587.5) * 86400000.0));
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(localMs, Qt::UTC);
    return dt.toLocalTime();
}

// QDateTime → модифицированная юлианская дата (double)
// Если dt равен ссылочной дате, возвращаем 0.
double CustomDateWidget::qDateTimeToModifiedJulian(const QDateTime &dt)
{
    QDateTime ref = dtToQDateTime(0);
    if (dt == ref)
        return 0.0;
    double jd = qDateTimeToJulian(dt);
    return jd - 2400000.5;
}

// Модифицированная юлианская дата (double) → QDateTime
// Если входное значение mjd равно 0 (или почти 0), возвращаем 01.01.2000.
QDateTime CustomDateWidget::modifiedJulianToQDateTime(double mjd)
{
    if (qFuzzyIsNull(mjd))
        return dtToQDateTime(0);
    double jd = mjd + 2400000.5;
    return julianToQDateTime(jd);
}

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