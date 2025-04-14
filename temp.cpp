#ifndef CUSTOMDATEWIDGET_H
#define CUSTOMDATEWIDGET_H

#include <QWidget>
#include <QDateTimeEdit>
#include <QVBoxLayout>

enum DateType {
    DT,    // Seconds since 00:00:00 January 1, 2000 (uint)
    UDT,   // Julian date (double)
    MUDT   // Modified Julian date (double)
};

class CustomDateWidget : public QWidget {
    Q_OBJECT
public:
    explicit CustomDateWidget(QWidget *parent = nullptr);

    // Set and get the current date type using exact string ("dt", "udt", "mudt")
    bool setDateType(const QString &type);
    DateType dateType() const;

    // Get the date as a string in the current type
    QString getNumericValue() const;

    // Set the date from a string in the current type
    void setNumericValue(const QString &value);

    // Direct access to the QDateTime
    QDateTime dateTime() const;
    void setDateTime(const QDateTime &dt);

signals:
    // Emitted when the date changes via UI or programmatically
    void valueChanged();

private:
    // Conversion functions: QDateTime to value
    QString dtToString(const QDateTime &dt) const;
    QString udtToString(const QDateTime &dt) const;
    QString mudtToString(const QDateTime &dt) const;

    // Conversion functions: Value to QDateTime
    QDateTime stringToDt(const QString &value) const;
    QDateTime stringToUdt(const QString &value) const;
    QDateTime stringToMudt(const QString &value) const;

    QDateTimeEdit *dateTimeEdit;
    DateType currentType;
};

#endif // CUSTOMDATEWIDGET_H







#include "CustomDateWidget.h"
#include <cmath> // for floor

CustomDateWidget::CustomDateWidget(QWidget *parent) : QWidget(parent), currentType(DT) {
    // Create the QDateTimeEdit with calendar popup
    dateTimeEdit = new QDateTimeEdit(this);
    dateTimeEdit->setCalendarPopup(true);
    dateTimeEdit->setTimeSpec(Qt::UTC); // Use UTC for consistency with Julian dates

    // Set up layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(dateTimeEdit);
    setLayout(layout);

    // Connect the dateTimeChanged signal to our valueChanged signal
    connect(dateTimeEdit, &QDateTimeEdit::dateTimeChanged, this, &CustomDateWidget::valueChanged);
}

bool CustomDateWidget::setDateType(const QString &type) {
    QString lowerType = type.toLower().trimmed();
    if (lowerType == "dt") {
        currentType = DT;
        return true;
    } else if (lowerType == "udt") {
        currentType = UDT;
        return true;
    } else if (lowerType == "mudt") {
        currentType = MUDT;
        return true;
    }
    return false; // Invalid type
}

DateType CustomDateWidget::dateType() const {
    return currentType;
}

QString CustomDateWidget::getNumericValue() const {
    QDateTime dt = dateTimeEdit->dateTime();
    switch (currentType) {
        case DT:
            return dtToString(dt);
        case UDT:
            return udtToString(dt);
        case MUDT:
            return mudtToString(dt);
    }
    return QString(); // Default empty string
}

void CustomDateWidget::setNumericValue(const QString &value) {
    QDateTime dt;
    switch (currentType) {
        case DT:
            dt = stringToDt(value);
            break;
        case UDT:
            dt = stringToUdt(value);
            break;
        case MUDT:
            dt = stringToMudt(value);
            break;
    }
    if (dt.isValid()) {
        dateTimeEdit->setDateTime(dt);
    }
}

QDateTime CustomDateWidget::dateTime() const {
    return dateTimeEdit->dateTime();
}

void CustomDateWidget::setDateTime(const QDateTime &dt) {
    dateTimeEdit->setDateTime(dt);
}

// Conversion: QDateTime to string for DT
QString CustomDateWidget::dtToString(const QDateTime &dt) const {
    QDateTime epoch(QDate(2000, 1, 1), QTime(0, 0, 0), Qt::UTC);
    qint64 secs = epoch.secsTo(dt);
    return QString::number(secs);
}

// Conversion: QDateTime to string for UDT
QString CustomDateWidget::udtToString(const QDateTime &dt) const {
    QDate date = dt.date();
    QTime time = dt.time();
    double jd = date.toJulianDay() - 0.5 + time.msecsSinceStartOfDay() / 86400000.0;
    return QString::number(jd, 'f', 6); // 6 decimal places for precision
}

// Conversion: QDateTime to string for MUDT
QString CustomDateWidget::mudtToString(const QDateTime &dt) const {
    QDate date = dt.date();
    QTime time = dt.time();
    double jd = date.toJulianDay() - 0.5 + time.msecsSinceStartOfDay() / 86400000.0;
    double mudt = jd - 2400000.5;
    return QString::number(mudt, 'f', 6); // 6 decimal places for precision
}

// Conversion: String to QDateTime for DT
QDateTime CustomDateWidget::stringToDt(const QString &value) const {
    bool ok;
    uint secs = value.toUInt(&ok);
    if (!ok) return QDateTime();
    QDateTime epoch(QDate(2000, 1, 1), QTime(0, 0, 0), Qt::UTC);
    return epoch.addSecs(secs);
}

// Conversion: String to QDateTime for UDT
QDateTime CustomDateWidget::stringToUdt(const QString &value) const {
    bool ok;
    double jd = value.toDouble(&ok);
    if (!ok) return QDateTime();
    int qt_jd = static_cast<int>(std::floor(jd + 0.5));
    double time_frac = jd - (qt_jd - 0.5);
    qint64 msecs = qRound64(time_frac * 86400000.0);
    QDate date = QDate::fromJulianDay(qt_jd);
    QTime time = QTime::fromMSecsSinceStartOfDay(msecs);
    return QDateTime(date, time, Qt::UTC);
}

// Conversion: String to QDateTime for MUDT
QDateTime CustomDateWidget::stringToMudt(const QString &value) const {
    bool ok;
    double mudt = value.toDouble(&ok);
    if (!ok) return QDateTime();
    double jd = mudt + 2400000.5;
    int qt_jd = static_cast<int>(std::floor(jd + 0.5));
    double time_frac = jd - (qt_jd - 0.5);
    qint64 msecs = qRound64(time_frac * 86400000.0);
    QDate date = QDate::fromJulianDay(qt_jd);
    QTime time = QTime::fromMSecsSinceStartOfDay(msecs);
    return QDateTime(date, time, Qt::UTC);
}