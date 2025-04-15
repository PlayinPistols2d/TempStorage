// Режим dt (секунды от 01.01.2000)
// Базовая дата: 2000-01-01 00:00:00 UTC
QDateTime CustomDateWidget::dtToQDateTime(qint64 seconds)
{
    QDateTime ref(QDate(2000, 1, 1), QTime(0, 0, 0), Qt::UTC);
    return ref.addSecs(seconds);
}

qint64 CustomDateWidget::QDateTimeToDt(const QDateTime &dt)
{
    QDateTime ref(QDate(2000, 1, 1), QTime(0, 0, 0), Qt::UTC);
    return ref.secsTo(dt);
}

// Функция перевода QDateTime → юлианская дата (JD)
// Используем стандартную формулу: JD = (UTC_msecs / 86400000) + 2440587.5
double CustomDateWidget::qDateTimeToJulian(const QDateTime &dt)
{
    // Приводим дату к UTC для вычислений
    QDateTime utc = dt.toUTC();
    double jd = static_cast<double>(utc.toMSecsSinceEpoch()) / 86400000.0 + 2440587.5;
    return jd;
}

// Обратное преобразование: JD (double) → QDateTime
// Расчитываем количество миллисекунд от Unix-эпохи по формуле
QDateTime CustomDateWidget::julianToQDateTime(double jd)
{
    // Вычисляем миллисекунды от Unix-эпохи в UTC
    qint64 msecs = static_cast<qint64>(qRound64((jd - 2440587.5) * 86400000.0));
    return QDateTime::fromMSecsSinceEpoch(msecs, Qt::UTC);
}

// Функция перевода QDateTime → модифицированная юлианская дата (MJD)
// По формуле: MJD = JD - 2400000.5. Если dt соответствует дате равной опорной (например, 01.01.2000) то результат будет корректен.
double CustomDateWidget::qDateTimeToModifiedJulian(const QDateTime &dt)
{
    double jd = qDateTimeToJulian(dt);
    return jd - 2400000.5;
}

// Обратное преобразование: MJD (double) → QDateTime
// Вычисляем JD, прибавляя 2400000.5, и затем вызываем julianToQDateTime.
QDateTime CustomDateWidget::modifiedJulianToQDateTime(double mjd)
{
    double jd = mjd + 2400000.5;
    return julianToQDateTime(jd);
}