#ifndef CUSTOMDATETIMEWIDGET_H
#define CUSTOMDATETIMEWIDGET_H

#include <QWidget>
#include <QDateTimeEdit>
#include <QString>
#include <QDateTime>

class CustomDateWidget : public QWidget
{
    Q_OBJECT
public:
    // Режимы работы виджета:
    // DT   — число секунд от 00:00:00 01.01.2000 (беззнаковое целое)
    // UDT  — юлианская дата с дробной частью (double)
    // MUDT — модифицированная юлианская дата с дробной частью (double)
    enum DateMode {
        DT,
        UDT,
        MUDT
    };

    explicit CustomDateWidget(QWidget* parent = nullptr);

    // Устанавливаем режим работы по входной строке ("dt", "udt" или "mudt")
    void setMode(const QString& modeStr);
    // Возвращает текущий режим в виде строки.
    QString currentMode() const;

    // Метод для установки числового значения (в зависимости от текущего режима)
    void setNumericValue(const QString& value);
    // Метод для получения числового представления даты/времени.
    QString getNumericValue() const;

    // Дополнительные методы для прямой работы с датой и временем
    QDateTime dateTime() const;
    void setDateTime(const QDateTime& dt);

signals:
    // Сигнал, испускаемый при изменении даты и времени.
    void dateTimeChanged(const QDateTime& dt);

private slots:
    // Слот для обработки изменения даты и времени в QDateTimeEdit.
    void onDateTimeChanged(const QDateTime& dt);

private:
    QDateTimeEdit* m_dateTimeEdit;  // Виджет для выбора даты и времени
    DateMode m_mode;                // Текущий режим работы

    // Эпоха для режима dt: 01.01.2000 00:00:00
    const QDateTime m_epoch = QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0));

    // Преобразования для режима dt (с датой и временем):
    QDateTime fromSeconds(quint64 seconds) const;
    quint64 toSeconds(const QDateTime& dt) const;

    // Преобразования для режима udt (юлианская дата с дробной частью):
    QDateTime fromJulian(double jd) const;
    double toJulian(const QDateTime& dt) const;

    // Преобразования для режима mudt (модифицированная юлианская дата с дробной частью):
    QDateTime fromModifiedJulian(double mjd) const;
    double toModifiedJulian(const QDateTime& dt) const;
};

#endif // CUSTOMDATETIMEWIDGET_H



#include "customdatewidget.h"
#include <QVBoxLayout>

CustomDateWidget::CustomDateWidget(QWidget* parent)
    : QWidget(parent),
      m_mode(DT) // по умолчанию режим dt
{
    // Используем QDateTimeEdit для ввода даты и времени
    m_dateTimeEdit = new QDateTimeEdit(this);
    m_dateTimeEdit->setCalendarPopup(true);
    // Задаём формат отображения: дата и время
    m_dateTimeEdit->setDisplayFormat("dd.MM.yyyy hh:mm:ss");
    m_dateTimeEdit->setDateTime(QDateTime::currentDateTime());

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_dateTimeEdit);
    setLayout(layout);

    // Связываем сигнал изменения даты/времени с соответствующим слотом
    connect(m_dateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &CustomDateWidget::onDateTimeChanged);
}

void CustomDateWidget::setMode(const QString &modeStr)
{
    if (modeStr.compare("dt", Qt::CaseInsensitive) == 0)
        m_mode = DT;
    else if (modeStr.compare("udt", Qt::CaseInsensitive) == 0)
        m_mode = UDT;
    else if (modeStr.compare("mudt", Qt::CaseInsensitive) == 0)
        m_mode = MUDT;
}

QString CustomDateWidget::currentMode() const
{
    switch (m_mode) {
        case DT:   return "dt";
        case UDT:  return "udt";
        case MUDT: return "mudt";
    }
    return "";
}

void CustomDateWidget::setNumericValue(const QString &value)
{
    bool ok = false;
    QDateTime dt;
    if (m_mode == DT) {
        quint64 seconds = value.toULongLong(&ok);
        if (ok) {
            dt = fromSeconds(seconds);
        }
    } else if (m_mode == UDT) {
        double jd = value.toDouble(&ok);
        if (ok) {
            dt = fromJulian(jd);
        }
    } else if (m_mode == MUDT) {
        double mjd = value.toDouble(&ok);
        if (ok) {
            dt = fromModifiedJulian(mjd);
        }
    }
    if (ok)
        m_dateTimeEdit->setDateTime(dt);
}

QString CustomDateWidget::getNumericValue() const
{
    QDateTime dt = m_dateTimeEdit->dateTime();
    if (m_mode == DT) {
        quint64 seconds = toSeconds(dt);
        return QString::number(seconds);
    } else if (m_mode == UDT) {
        double jd = toJulian(dt);
        return QString::number(jd, 'f', 6);
    } else if (m_mode == MUDT) {
        double mjd = toModifiedJulian(dt);
        return QString::number(mjd, 'f', 6);
    }
    return "";
}

QDateTime CustomDateWidget::dateTime() const
{
    return m_dateTimeEdit->dateTime();
}

void CustomDateWidget::setDateTime(const QDateTime &dt)
{
    m_dateTimeEdit->setDateTime(dt);
}

void CustomDateWidget::onDateTimeChanged(const QDateTime &dt)
{
    emit dateTimeChanged(dt);
}

// Преобразования для режима dt (с датой и временем):
QDateTime CustomDateWidget::fromSeconds(quint64 seconds) const
{
    return m_epoch.addSecs(static_cast<qint64>(seconds));
}

quint64 CustomDateWidget::toSeconds(const QDateTime &dt) const
{
    return static_cast<quint64>(m_epoch.secsTo(dt));
}

// Преобразования для режима udt: учитываем дробную часть дня
QDateTime CustomDateWidget::fromJulian(double jd) const
{
    // Разбиваем значение на целую и дробную части
    int jday = static_cast<int>(jd);
    double fraction = jd - jday;
    QDate date = QDate::fromJulianDay(jday);
    int seconds = static_cast<int>(fraction * 86400);
    QTime time = QTime(0, 0, 0).addSecs(seconds);
    return QDateTime(date, time);
}

double CustomDateWidget::toJulian(const QDateTime &dt) const
{
    int jday = dt.date().toJulianDay();
    int seconds = dt.time().hour() * 3600 + dt.time().minute() * 60 + dt.time().second();
    double fraction = static_cast<double>(seconds) / 86400.0;
    return jday + fraction;
}

// Преобразования для режима mudt: MJD = JD - 2400000.5
QDateTime CustomDateWidget::fromModifiedJulian(double mjd) const
{
    double jd = mjd + 2400000.5;
    return fromJulian(jd);
}

double CustomDateWidget::toModifiedJulian(const QDateTime &dt) const
{
    double jd = toJulian(dt);
    return jd - 2400000.5;
}