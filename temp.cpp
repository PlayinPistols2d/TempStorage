#ifndef CUSTOMDATEWIDGET_H
#define CUSTOMDATEWIDGET_H

#include <QWidget>
#include <QDateEdit>
#include <QString>

class CustomDateWidget : public QWidget
{
    Q_OBJECT
public:
    // Режимы работы виджета
    enum DateMode {
        DT,    // dt: число секунд с 00:00:00 01.01.2000 (беззнаковое целое)
        UDT,   // udt: юлианская дата (double)
        MUDT   // mudt: модифицированная юлианская дата (double)
    };

    explicit CustomDateWidget(QWidget* parent = nullptr);

    // Устанавливает режим работы, принимая строку ("dt", "udt" или "mudt").
    void setMode(const QString& modeStr);
    // Возвращает текущий режим в виде строки.
    QString currentMode() const;

    // Устанавливает числовое значение даты в виджете согласно текущему режиму.
    void setNumericValue(const QString& value);
    // Возвращает числовое значение даты согласно текущему режиму.
    QString getNumericValue() const;

    // Дополнительные вспомогательные методы для прямой работы с датой.
    QDate date() const;
    void setDate(const QDate& date);

signals:
    // Сигнал об изменении даты (при выборе через календарь)
    void dateChanged(const QDate& date);

private slots:
    // Слот для обработки изменения даты в QDateEdit.
    void onDateChanged(const QDate& date);

private:
    QDateEdit* m_dateEdit;  // Основной виджет для выбора даты
    DateMode m_mode;        // Текущий режим работы

    // Вспомогательные методы для конвертации в режиме dt.
    QDate fromSeconds(quint64 seconds) const;
    quint64 toSeconds(const QDate& date) const;

    // Вспомогательные методы для конвертации в режиме udt (юлианская дата).
    QDate fromJulian(double jd) const;
    double toJulian(const QDate& date) const;

    // Вспомогательные методы для конвертации в режиме mudt (модифицированная юлианская дата).
    QDate fromModifiedJulian(double mjd) const;
    double toModifiedJulian(const QDate& date) const;

    // Эпоха для режима dt: 01.01.2000
    const QDate m_epoch = QDate(2000, 1, 1);
};

#endif // CUSTOMDATEWIDGET_H





#include "customdatewidget.h"
#include <QDateTime>
#include <QVBoxLayout>

CustomDateWidget::CustomDateWidget(QWidget* parent)
    : QWidget(parent),
      m_mode(DT) // по умолчанию режим dt
{
    // Инициализация QDateEdit с выпадающим календарем и форматом отображения
    m_dateEdit = new QDateEdit(this);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("dd.MM.yyyy");
    m_dateEdit->setDate(QDate::currentDate());

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_dateEdit);
    setLayout(layout);

    // Связь сигнала изменения даты с нашим слотом
    connect(m_dateEdit, &QDateEdit::dateChanged, this, &CustomDateWidget::onDateChanged);
}

void CustomDateWidget::setMode(const QString &modeStr)
{
    // Устанавливаем режим в зависимости от входной строки (без учёта регистра).
    if (modeStr.compare("dt", Qt::CaseInsensitive) == 0) {
        m_mode = DT;
    }
    else if (modeStr.compare("udt", Qt::CaseInsensitive) == 0) {
        m_mode = UDT;
    }
    else if (modeStr.compare("mudt", Qt::CaseInsensitive) == 0) {
        m_mode = MUDT;
    }
    // При неправильном режиме можно добавить логирование ошибки или сигнал об ошибке.
}

QString CustomDateWidget::currentMode() const
{
    switch (m_mode) {
        case DT:    return "dt";
        case UDT:   return "udt";
        case MUDT:  return "mudt";
        default:    return "";
    }
}

void CustomDateWidget::setNumericValue(const QString &value)
{
    bool ok = false;
    QDate parsedDate;
    // В зависимости от режима парсим входное числовое значение
    if (m_mode == DT) {
        quint64 seconds = value.toULongLong(&ok);
        if (ok) {
            parsedDate = fromSeconds(seconds);
        }
    } else if (m_mode == UDT) {
        double jd = value.toDouble(&ok);
        if (ok) {
            parsedDate = fromJulian(jd);
        }
    } else if (m_mode == MUDT) {
        double mjd = value.toDouble(&ok);
        if (ok) {
            parsedDate = fromModifiedJulian(mjd);
        }
    }
    // Если конвертация прошла успешно, устанавливаем дату в виджете.
    if (ok)
        m_dateEdit->setDate(parsedDate);
    // При неудачном парсинге можно добавить обработку ошибок.
}

QString CustomDateWidget::getNumericValue() const
{
    QDate current = m_dateEdit->date();
    if (m_mode == DT) {
        quint64 seconds = toSeconds(current);
        return QString::number(seconds);
    } else if (m_mode == UDT) {
        double jd = toJulian(current);
        // Форматируем число с фиксированным количеством знаков после запятой.
        return QString::number(jd, 'f', 6);
    } else if (m_mode == MUDT) {
        double mjd = toModifiedJulian(current);
        return QString::number(mjd, 'f', 6);
    }
    return "";
}

QDate CustomDateWidget::date() const
{
    return m_dateEdit->date();
}

void CustomDateWidget::setDate(const QDate &date)
{
    m_dateEdit->setDate(date);
}

void CustomDateWidget::onDateChanged(const QDate &date)
{
    emit dateChanged(date);
}

// Функция для режима dt: преобразование секунд с эпохи (01.01.2000) в QDate
QDate CustomDateWidget::fromSeconds(quint64 seconds) const
{
    QDateTime epochDateTime(m_epoch);
    QDateTime result = epochDateTime.addSecs(static_cast<qint64>(seconds));
    return result.date();
}

// Обратное преобразование: из даты в число секунд от эпохи
quint64 CustomDateWidget::toSeconds(const QDate &date) const
{
    QDateTime epochDateTime(m_epoch);
    QDateTime dateTime(date);
    return static_cast<quint64>(epochDateTime.secsTo(dateTime));
}

// Преобразование для режима udt: из юлианского числа (double) в QDate.
// Так как QDate работает с целыми юлианскими днями, округляем значение.
QDate CustomDateWidget::fromJulian(double jd) const
{
    int julianDay = static_cast<int>(jd + 0.5);
    return QDate::fromJulianDay(julianDay);
}

// Преобразование QDate в юлианское число (double)
// QDate::toJulianDay() возвращает целое число – при необходимости дробную часть можно добавить.
double CustomDateWidget::toJulian(const QDate &date) const
{
    return static_cast<double>(date.toJulianDay());
}

// Преобразование для режима mudt: модифицированная юлианская дата.
// Определение: MJD = JD - 2400000.5.
QDate CustomDateWidget::fromModifiedJulian(double mjd) const
{
    double jd = mjd + 2400000.5;
    int julianDay = static_cast<int>(jd + 0.5);
    return QDate::fromJulianDay(julianDay);
}

double CustomDateWidget::toModifiedJulian(const QDate &date) const
{
    double jd = static_cast<double>(date.toJulianDay());
    return jd - 2400000.5;
}