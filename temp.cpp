// Функция, возвращающая опорную дату (01.01.2000 00:00:00) в локальном часовом поясе.
QDateTime CustomDateWidget::dtToQDateTime(qint64 seconds)
{
    // Используем локальное время для справочной даты.
    QDateTime ref(QDate(2000, 1, 1), QTime(0, 0, 0), Qt::LocalTime);
    return ref.addSecs(seconds);
}

// Преобразование: QDateTime → "юлианское" значение, где 0 означает 01.01.2000 00:00:00.
// Теперь вычисляем разницу в миллисекундах между dt и опорной датой, затем переводим в дни.
double CustomDateWidget::qDateTimeToJulian(const QDateTime &dt)
{
    QDateTime refLocal = dtToQDateTime(0);
    qint64 diffMs = dt.toMSecsSinceEpoch() - refLocal.toMSecsSinceEpoch();
    return static_cast<double>(diffMs) / 86400000.0;
}

// Преобразование: "юлианское" значение → QDateTime.
// Если jd близко к нулю, то возвращаем опорную дату.
// Иначе, вычисляем смещение в миллисекундах (количество дней * мс в дне)
// и прибавляем его к опорной дате.
QDateTime CustomDateWidget::julianToQDateTime(double jd)
{
    QDateTime refLocal = dtToQDateTime(0);
    if (qFuzzyIsNull(jd))
        return refLocal; // Если jd ≈ 0,  возвращаем опорную дату

    qint64 msOffset = static_cast<qint64>(qRound64(jd * 86400000.0));
    return refLocal.addMSecs(msOffset);
}