// Преобразование: QDateTime → юлианская дата (double)
// Стандартная формула: JD = (секунд с Unix-эпохи)/86400 + 2440587.5
// Здесь используем миллисекунды для большей точности.
double CustomDateWidget::qDateTimeToJulian(const QDateTime &dt)
{
    // Приводим дату к UTC
    QDateTime utc = dt.toUTC();
    double jd = (static_cast<double>(utc.toMSecsSinceEpoch()) / 86400000.0) + 2440587.5;
    return jd;
}

// Преобразование: юлианская дата (double) → QDateTime
// Обратная формула: UnixMSec = (JD - 2440587.5) * 86400000
// Результат создается в UTC, затем переводится в локальное время.
QDateTime CustomDateWidget::julianToQDateTime(double jd)
{
    double msecs = (jd - 2440587.5) * 86400000.0;
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(qRound64(msecs)), Qt::UTC);
    return dt.toLocalTime();
}