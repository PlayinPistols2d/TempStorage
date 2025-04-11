#include <QLineEdit>
#include <QWidget>
#include <QString>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QtMath>
#include <QDebug>

/*
 * Функция обработки строки.
 * При обнаружении нужного ключевого слова скрывает QLineEdit и показывает
 * соответствующий виджет для работы с датой в заданном формате.
 */
void handleDateInput(const QString &input,
                     QLineEdit* lineEdit,
                     QWidget* dtWidget,    // виджет для формата dt (секунды с 01.01.2000)
                     QWidget* udtWidget,   // виджет для формата udt (юлианская дата)
                     QWidget* mudtWidget)  // виджет для формата mudt (модифицированная юлианская дата)
{
    // Приоритет поиска: сначала ищем более специфичные варианты (mudt, udt),
    // затем – более общий (dt). Это нужно, чтобы не перепутать udt или mudt с dt.
    if(input.contains("mudt", Qt::CaseInsensitive)) {
         lineEdit->hide();
         mudtWidget->show();
         dtWidget->hide();
         udtWidget->hide();
         qDebug() << "Показан виджет для модифицированной юлианской даты (mudt)";
    } else if(input.contains("udt", Qt::CaseInsensitive)) {
         lineEdit->hide();
         udtWidget->show();
         dtWidget->hide();
         mudtWidget->hide();
         qDebug() << "Показан виджет для юлианской даты (udt)";
    } else if(input.contains("dt", Qt::CaseInsensitive)) {
         lineEdit->hide();
         dtWidget->show();
         udtWidget->hide();
         mudtWidget->hide();
         qDebug() << "Показан виджет для dt (секунды с 01.01.2000)";
    } else {
         // Если ключевые фразы не обнаружены – показываем стандартный QLineEdit
         lineEdit->show();
         dtWidget->hide();
         udtWidget->hide();
         mudtWidget->hide();
         qDebug() << "Ключевые фразы не найдены. Показан стандартный lineEdit.";
    }
}

/*
 * Функции для преобразования дат.
 *
 * 1. dt (double) представляет собой количество секунд, прошедших от 01.01.2000 00:00:00.
 *    Используем QDateTime для удобства работы с датой/временем.
 */

// Преобразование dt -> QDateTime (dt: кол-во секунд от базовой даты)
QDateTime dtToQDateTime(double dt)
{
    QDateTime base(QDate(2000, 1, 1), QTime(0, 0, 0), Qt::UTC);
    return base.addSecs(static_cast<qint64>(dt));
}

// Преобразование QDateTime -> dt
double qDateTimeToDt(const QDateTime &dateTime)
{
    QDateTime base(QDate(2000, 1, 1), QTime(0, 0, 0), Qt::UTC);
    return base.secsTo(dateTime);
}

/*
 * 2. Перевод QDateTime в юлианскую дату (JD) и обратно.
 *
 * Формула вычисления JD для даты в григорианском календаре:
 *   JD = ⌊365.25*(year + 4716)⌋ + ⌊30.6001*(month+1)⌋ + day + B - 1524.5
 * где B = 2 - A + ⌊A/4⌋, A = ⌊year/100⌋.
 * Временная дробная часть добавляется в виде доли суток.
 */

// Преобразование QDateTime -> юлианская дата (JD)
double qDateTimeToJD(const QDateTime &dateTime)
{
    QDate date = dateTime.date();
    int year = date.year();
    int month = date.month();
    int day = date.day();

    // Если месяц - январь или февраль, корректируем год и месяц
    if (month <= 2) {
        year -= 1;
        month += 12;
    }
    int A = year / 100;
    int B = 2 - A + (A / 4);

    // Вычисляем JD для 0 часов UT
    double jd = int(365.25 * (year + 4716)) + int(30.6001 * (month + 1)) + day + B - 1524.5;

    // Добавляем дробную часть дня с учетом времени
    QTime time = dateTime.time();
    double dayFraction = (time.hour() + time.minute() / 60.0 + time.second() / 3600.0) / 24.0;
    return jd + dayFraction;
}

// Преобразование юлианской даты (JD) -> QDateTime
QDateTime jdToQDateTime(double jd)
{
    // Алгоритм перевода JD в григорианскую дату (с корректировкой)
    double Z = std::floor(jd + 0.5);
    double F = (jd + 0.5) - Z;
    double A = Z;
    if(Z >= 2299161) {
        double alpha = std::floor((Z - 1867216.25) / 36524.25);
        A = Z + 1 + alpha - std::floor(alpha / 4);
    }
    double B = A + 1524;
    double C = std::floor((B - 122.1) / 365.25);
    double D = std::floor(365.25 * C);
    double E = std::floor((B - D) / 30.6001);
    int day = static_cast<int>(B - D - std::floor(30.6001 * E));
    int month = (E < 14) ? static_cast<int>(E - 1) : static_cast<int>(E - 13);
    int year = (month > 2) ? static_cast<int>(C - 4716) : static_cast<int>(C - 4715);

    // Вычисляем время (дробная часть суток)
    double secondsInDay = F * 86400;
    int hours = static_cast<int>(secondsInDay / 3600);
    int minutes = static_cast<int>((secondsInDay - hours * 3600) / 60);
    int seconds = static_cast<int>(secondsInDay - hours * 3600 - minutes * 60);

    QDate date(year, month, day);
    QTime time(hours, minutes, seconds);
    return QDateTime(date, time, Qt::UTC);
}

/*
 * 3. Преобразование в модифицированную юлианскую дату (MJD) и обратно.
 * MJD определяется как: MJD = JD - 2400000.5
 */

// Преобразование QDateTime -> MJD (модифицированная юлианская дата)
double qDateTimeToMJD(const QDateTime &dateTime)
{
    double jd = qDateTimeToJD(dateTime);
    return jd - 2400000.5;
}

// Преобразование MJD -> QDateTime
QDateTime mjdToQDateTime(double mjd)
{
    double jd = mjd + 2400000.5;
    return jdToQDateTime(jd);
}

/*
 * Пример использования:
 *
 * Предположим, что у вас есть объекты:
 *     QLineEdit*      myLineEdit;
 *     QWidget*        dtWidget;
 *     QWidget*        udtWidget;
 *     QWidget*        mudtWidget;
 *
 * Тогда, при вводе строки можно вызвать:
 *
 *     handleDateInput(userInputString, myLineEdit, dtWidget, udtWidget, mudtWidget);
 *
 * Для преобразования даты:
 *
 *     // dt -> QDateTime
 *     QDateTime dtTime = dtToQDateTime(3600); // например, 3600 секунд после 01.01.2000
 *
 *     // QDateTime -> dt
 *     double dtVal = qDateTimeToDt(QDateTime::currentDateTimeUtc());
 *
 *     // QDateTime -> юлианская дата (udt)
 *     double jd = qDateTimeToJD(QDateTime::currentDateTimeUtc());
 *
 *     // Юлианская дата -> QDateTime
 *     QDateTime fromJD = jdToQDateTime(jd);
 *
 *     // QDateTime -> MJD
 *     double mjd = qDateTimeToMJD(QDateTime::currentDateTimeUtc());
 *
 *     // MJD -> QDateTime
 *     QDateTime fromMJD = mjdToQDateTime(mjd);
 *
 * Эти функции можно расширять, дополнять проверками и подстраивать под особенности вашего интерфейса.
 */