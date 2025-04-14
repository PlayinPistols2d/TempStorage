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

    // Set and get the current date type
    void setDateType(DateType type);
    DateType dateType() const;

    // Get the date as a numeric value in the current type
    double getNumericValue() const;

    // Set the date from a numeric value in the current type
    void setNumericValue(double value);

    // Parse input string and set the value based on current type
    bool parseAndSetValue(const QString &input);

    // Direct access to the QDateTime
    QDateTime dateTime() const;
    void setDateTime(const QDateTime &dt);

signals:
    // Emitted when the date changes via UI or programmatically
    void valueChanged();

private:
    QDateTimeEdit *dateTimeEdit;
    DateType currentType;
};

#endif // CUSTOMDATEWIDGET_H




#include "CustomDateWidget.h"
#include <cmath> // for floor
#include <QStringList>

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

void CustomDateWidget::setDateType(DateType type) {
    currentType = type;
}

DateType CustomDateWidget::dateType() const {
    return currentType;
}

double CustomDateWidget::getNumericValue() const {
    QDateTime dt = dateTimeEdit->dateTime();
    switch (currentType) {
        case DT: {
            // Seconds since 00:00:00 January 1, 2000 UTC
            QDateTime epoch(QDate(2000, 1, 1), QTime(0, 0, 0), Qt::UTC);
            qint64 secs = epoch.secsTo(dt);
            return static_cast<double>(secs);
        }
        case UDT: {
            // Julian date: days since noon January 1, 4713 BCE
            QDate date = dt.date();
            QTime time = dt.time();
            // Qt's toJulianDay starts at midnight, adjust to astronomical JD
            double jd = date.toJulianDay() - 0.5 + time.msecsSinceStartOfDay() / 86400000.0;
            return jd;
        }
        case MUDT: {
            // Modified Julian date: JD - 2400000.5
            QDate date = dt.date();
            QTime time = dt.time();
            double jd = date.toJulianDay() - 0.5 + time.msecsSinceStartOfDay() / 86400000.0;
            return jd - 2400000.5;
        }
    }
    return 0.0; // Default return value
}

void CustomDateWidget::setNumericValue(double value) {
    QDateTime dt;
    switch (currentType) {
        case DT: {
            // Interpret value as seconds since 00:00:00 January 1, 2000 UTC
            uint secs = static_cast<uint>(value);
            QDateTime epoch(QDate(2000, 1, 1), QTime(0, 0, 0), Qt::UTC);
            dt = epoch.addSecs(secs);
            break;
        }
        case UDT: {
            // Interpret value as Julian date
            double jd = value;
            int qt_jd = static_cast<int>(std::floor(jd + 0.5));
            double time_frac = jd - (qt_jd - 0.5);
            qint64 msecs = qRound64(time_frac * 86400000.0);
            QDate date = QDate::fromJulianDay(qt_jd);
            QTime time = QTime::fromMSecsSinceStartOfDay(msecs);
            dt = QDateTime(date, time, Qt::UTC);
            break;
        }
        case MUDT: {
            // Interpret value as modified Julian date
            double mudt = value;
            double jd = mudt + 2400000.5;
            int qt_jd = static_cast<int>(std::floor(jd + 0.5));
            double time_frac = jd - (qt_jd - 0.5);
            qint64 msecs = qRound64(time_frac * 86400000.0);
            QDate date = QDate::fromJulianDay(qt_jd);
            QTime time = QTime::fromMSecsSinceStartOfDay(msecs);
            dt = QDateTime(date, time, Qt::UTC);
            break;
        }
    }
    dateTimeEdit->setDateTime(dt);
}

bool CustomDateWidget::parseAndSetValue(const QString &input) {
    // Expected format: "dt=<value>,udt=<value>,mudt=<value>"
    QStringList parts = input.split(',', Qt::SkipEmptyParts);
    double dtValue = 0.0, udtValue = 0.0, mudtValue = 0.0;
    bool foundDt = false, foundUdt = false, foundMudt = false;

    for (const QString &part : parts) {
        QString trimmed = part.trimmed();
        if (trimmed.startsWith("dt=", Qt::CaseInsensitive)) {
            bool ok;
            dtValue = trimmed.mid(3).toDouble(&ok);
            if (ok) foundDt = true;
        } else if (trimmed.startsWith("udt=", Qt::CaseInsensitive)) {
            bool ok;
            udtValue = trimmed.mid(4).toDouble(&ok);
            if (ok) foundUdt = true;
        } else if (trimmed.startsWith("mudt=", Qt::CaseInsensitive)) {
            bool ok;
            mudtValue = trimmed.mid(5).toDouble(&ok);
            if (ok) foundMudt = true;
        }
    }

    // Check if the required value was found and set it
    switch (currentType) {
        case DT:
            if (foundDt) {
                setNumericValue(dtValue);
                return true;
            }
            break;
        case UDT:
            if (foundUdt) {
                setNumericValue(udtValue);
                return true;
            }
            break;
        case MUDT:
            if (foundMudt) {
                setNumericValue(mudtValue);
                return true;
            }
            break;
    }

    return false; // Return false if the relevant value wasn't found or parsed
}

QDateTime CustomDateWidget::dateTime() const {
    return dateTimeEdit->dateTime();
}

void CustomDateWidget::setDateTime(const QDateTime &dt) {
    dateTimeEdit->setDateTime(dt);
}
