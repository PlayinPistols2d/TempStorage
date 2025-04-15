#include <QDateTime>
#include <QtMath>
#include <cmath>

// -------------------- DT: СЕКУНДЫ от 2000-01-01 UTC --------------------

QDateTime CustomDateWidget::dtToQDateTime(qint64 seconds)
{
    static const QDateTime dtEpoch(QDate(2000, 1, 1), QTime(0, 0), Qt::UTC);
    QDateTime dt = dtEpoch.addSecs(seconds);
    dt.setTimeSpec(Qt::UTC);
    return dt;
}

qint64 CustomDateWidget::QDateTimeToDt(const QDateTime &dt)
{
    static const QDateTime dtEpoch(QDate(2000, 1, 1), QTime(0, 0), Qt::UTC);
    return dtEpoch.secsTo(dt.toUTC());
}

// -------------------- UDT: Юлианская дата (дни) --------------------

QDateTime CustomDateWidget::julianToQDateTime(double jd)
{
    static const QDateTime julianEpoch(QDate(1970, 1, 1), QTime(0, 0), Qt::UTC);

    if (!std::isfinite(jd) || jd <= 0.0)
        return julianEpoch;

    double offset = jd - 2440587.5;
    qint64 days = static_cast<qint64>(offset);
    double fractional = offset - static_cast<double>(days);
    qint64 msecs = static_cast<qint64>(std::round(fractional * 86400000.0));

    QDateTime dt = julianEpoch.addDays(days).addMSecs(msecs);
    dt.setTimeSpec(Qt::UTC);
    return dt;
}

double CustomDateWidget::qDateTimeToJulian(const QDateTime &dateTime)
{
    static const QDateTime julianEpoch(QDate(1970, 1, 1), QTime(0, 0), Qt::UTC);

    QDateTime utc = dateTime.toUTC();
    qint64 days = julianEpoch.daysTo(utc);
    qint64 msecsInDay = 86400000;
    qint64 msecsSinceMidnight = utc.time().msecsSinceStartOfDay();

    double fractional = static_cast<double>(msecsSinceMidnight) / static_cast<double>(msecsInDay);
    return 2440587.5 + static_cast<double>(days) + fractional;
}

// -------------------- MUDT: Модифицированная юлианская дата (дни) --------------------

QDateTime CustomDateWidget::modifiedJulianToQDateTime(double mjd)
{
    static const QDateTime mjdEpoch(QDate(1858, 11, 17), QTime(0, 0), Qt::UTC);

    if (!std::isfinite(mjd) || mjd <= 0.0)
        return mjdEpoch;

    qint64 days = static_cast<qint64>(mjd);
    double fractional = mjd - static_cast<double>(days);
    qint64 msecs = static_cast<qint64>(std::round(fractional * 86400000.0));

    QDateTime dt = mjdEpoch.addDays(days).addMSecs(msecs);
    dt.setTimeSpec(Qt::UTC);
    return dt;
}

double CustomDateWidget::qDateTimeToModifiedJulian(const QDateTime &dateTime)
{
    static const QDateTime mjdEpoch(QDate(1858, 11, 17), QTime(0, 0), Qt::UTC);

    QDateTime utc = dateTime.toUTC();
    qint64 days = mjdEpoch.daysTo(utc);
    qint64 msecsInDay = 86400000;
    qint64 msecsSinceMidnight = utc.time().msecsSinceStartOfDay();

    double fractional = static_cast<double>(msecsSinceMidnight) / static_cast<double>(msecsInDay);
    return static_cast<double>(days) + fractional;
}