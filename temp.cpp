// --------------------- Режим dt (секунды от 01.01.2000) ---------------------

// Базовая дата для режима dt (опорная дата 2000-01-01 00:00:00 UTC)
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

// --------------------- Режим udt (Юлианская дата, JD в днях) ---------------------

// Преобразование QDateTime → стандартное значение JD (в днях)
// Используется стандартная формула:  
//  JD = (UTC_msecs / 86400000.0) + 2440587.5
double CustomDateWidget::qDateTimeToJulian(const QDateTime &dt)
{
    // Приводим дату к UTC для вычислений
    QDateTime utc = dt.toUTC();
    double jd = static_cast<double>(utc.toMSecsSinceEpoch()) / 86400000.0 + 2440587.5;
    return jd;
}

// Обратное преобразование: JD (в днях) → QDateTime
// По формуле: msecs = (JD - 2440587.5) * 86400000.0
QDateTime CustomDateWidget::julianToQDateTime(double jd)
{
    qint64 msecs = static_cast<qint64>(qRound64((jd - 2440587.5) * 86400000.0));
    return QDateTime::fromMSecsSinceEpoch(msecs, Qt::UTC);
}

// --------------------- Режим mudt (Модифицированная юлианская дата, MJD в днях) ---------------------

// Преобразование QDateTime → MJD (в днях)
// По стандарту MJD = JD - 2400000.5
double CustomDateWidget::qDateTimeToModifiedJulian(const QDateTime &dt)
{
    double jd = qDateTimeToJulian(dt);
    return jd - 2400000.5;
}

// Обратное преобразование: MJD (в днях) → QDateTime
// Вычисляем сначала JD как MJD + 2400000.5, затем QDateTime
QDateTime CustomDateWidget::modifiedJulianToQDateTime(double mjd)
{
    double jd = mjd + 2400000.5;
    return julianToQDateTime(jd);
}