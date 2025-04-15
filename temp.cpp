// UDT (Юлианская дата): работает в днях, базовая дата: 4713-01-01 BCE = JD 0
// Мы начнем с JD эпохи Unix (1970-01-01 = JD 2440587.5)
static const QDateTime julianEpoch = QDateTime(QDate(1970, 1, 1), QTime(0, 0), Qt::UTC);

// MJD базируется на 1858-11-17 = JD 2400000.5
static const QDateTime mjdEpoch = QDateTime(QDate(1858, 11, 17), QTime(0, 0, 0), Qt::UTC);

double CustomDateWidget::qDateTimeToJulian(const QDateTime &dateTime)
{
    QDateTime utc = dateTime.toUTC();

    qint64 days = julianEpoch.daysTo(utc);
    qint64 msecsInDay = 86400000;
    qint64 msecsSinceMidnight = utc.time().msecsSinceStartOfDay();

    double fractionalDay = static_cast<double>(msecsSinceMidnight) / static_cast<double>(msecsInDay);
    double jd = 2440587.5 + static_cast<double>(days) + fractionalDay;

    return jd;
}

QDateTime CustomDateWidget::julianToQDateTime(double jd)
{
    double jdOffset = jd - 2440587.5;
    qint64 days = static_cast<qint64>(jdOffset);
    double fractional = jdOffset - static_cast<double>(days);
    qint64 msecs = static_cast<qint64>(std::round(fractional * 86400000.0));

    QDateTime result = julianEpoch.addDays(days).addMSecs(msecs);
    return result;
}

double CustomDateWidget::qDateTimeToModifiedJulian(const QDateTime &dateTime)
{
    QDateTime utc = dateTime.toUTC();

    qint64 days = mjdEpoch.daysTo(utc);
    qint64 msecsInDay = 86400000;
    qint64 msecsSinceMidnight = utc.time().msecsSinceStartOfDay();

    double fractionalDay = static_cast<double>(msecsSinceMidnight) / static_cast<double>(msecsInDay);
    double mjd = static_cast<double>(days) + fractionalDay;

    return mjd;
}

QDateTime CustomDateWidget::modifiedJulianToQDateTime(double mjd)
{
    qint64 days = static_cast<qint64>(mjd);
    double fractional = mjd - static_cast<double>(days);
    qint64 msecs = static_cast<qint64>(std::round(fractional * 86400000.0));

    QDateTime result = mjdEpoch.addDays(days).addMSecs(msecs);
    return result;
}
