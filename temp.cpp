#ifndef CUSTOMDATEWIDGET_H
#define CUSTOMDATEWIDGET_H

#include <QWidget>
#include <QString>
#include <QDateTime>

class QLineEdit;
class QDateTimeEdit;
class QLabel;
class QFrame;

class CustomDateWidget : public QWidget
{
    Q_OBJECT

public:
    enum DateMode {
        DtMode,    // dt: число секунд от 01.01.2000
        UdtMode,   // udt: юлианская дата (double)
        MudtMode,  // mudt: модифицированная юлианская дата (double)
        NoneMode   // режим не установлен
    };

    explicit CustomDateWidget(QWidget *parent = nullptr);

    // Устанавливает режим по строке ("dt", "udt" или "mudt", без лишних символов)
    void setMode(const QString &modeStr);

    // Возвращает текущее значение даты в числовом формате в виде строки.
    // Для режима dt – это целое число, для udt/mudt – double с 8 знаками после запятой.
    QString getNumericValue();

    // Устанавливает значение даты в виджете по входной строке.
    // Если режим dt – строка интерпретируется как число (возможно с плавающей точкой) секунд от 01.01.2000.
    // Для udt/mudt – как double.
    void setNumericValue(const QString &value);

    // Преобразования – они работают по-разному в зависимости от текущего режима.
    QVariant convertDateValue(const QDateTime &dt);
    QDateTime convertToQDateTime(const QVariant &value);

    // Функции для режима dt:
    static QDateTime dtToQDateTime(qint64 seconds);
    static qint64 QDateTimeToDt(const QDateTime &dt);

    // Преобразования для режима udt (юлианская дата):
    // Стандартная формула: JD = (ms_local/86400000.0) + 2440587.5.
    // Если значение равно 0 (или dt равен 01.01.2000), возвращается 0.
    static double qDateTimeToJulian(const QDateTime &dt);
    static QDateTime julianToQDateTime(double jd);

    // Для mudt (модифицированная юлианская дата):
    // MJD = JD - 2400000.5.
    static double qDateTimeToModifiedJulian(const QDateTime &dt);
    static QDateTime modifiedJulianToQDateTime(double mjd);

signals:
    // Сигнал, который испускается при изменении даты.
    void dateChanged(const QDateTime &dateTime);

private:
    void updateWidgetDisplay();

    QLabel *modeLabel;         // Информация о текущем режиме
    QLineEdit *lineEdit;       // Для случая, когда режим не установлен
    QDateTimeEdit *dateTimeEdit; // Основной элемент для ввода даты
    QFrame *frame;             // Визуальное обрамление

    DateMode currentMode;
};

#endif // CUSTOMDATEWIDGET_H




#include "CustomDateWidget.h"

#include <QLineEdit>
#include <QDateTimeEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QFrame>
#include <QDateTime>
#include <QDate>
#include <QString>
#include <QtMath>
#include <QtGlobal> // Для qFuzzyIsNull

// Константа для ссылочного времени: 01.01.2000 00:00:00
static QDateTime refDateTime()
{
    return QDateTime(QDate(2000, 1, 1), QTime(0, 0, 0));
}

CustomDateWidget::CustomDateWidget(QWidget *parent)
    : QWidget(parent), currentMode(NoneMode)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    frame = new QFrame(this);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setFrameShadow(QFrame::Raised);

    QVBoxLayout *frameLayout = new QVBoxLayout(frame);

    modeLabel = new QLabel("Текущий режим: не установлен", frame);
    frameLayout->addWidget(modeLabel);

    lineEdit = new QLineEdit(frame);
    frameLayout->addWidget(lineEdit);

    dateTimeEdit = new QDateTimeEdit(frame);
    dateTimeEdit->setCalendarPopup(true);
    dateTimeEdit->setDisplayFormat("dd.MM.yyyy HH:mm:ss");
    frameLayout->addWidget(dateTimeEdit);
    dateTimeEdit->hide();

    // Пробрасываем сигнал от QDateTimeEdit наружу
    connect(dateTimeEdit, &QDateTimeEdit::dateTimeChanged,
            this, &CustomDateWidget::dateChanged);

    mainLayout->addWidget(frame);
    setLayout(mainLayout);
}

void CustomDateWidget::setMode(const QString &modeStr)
{
    if (modeStr.compare("dt", Qt::CaseInsensitive) == 0) {
        currentMode = DtMode;
        modeLabel->setText("Текущий режим: dt (секунд с 01.01.2000)");
    } else if (modeStr.compare("udt", Qt::CaseInsensitive) == 0) {
        currentMode = UdtMode;
        modeLabel->setText("Текущий режим: udt (юлианская дата)");
    } else if (modeStr.compare("mudt", Qt::CaseInsensitive) == 0) {
        currentMode = MudtMode;
        modeLabel->setText("Текущий режим: mudt (модифицированная юлианская дата)");
    } else {
        currentMode = NoneMode;
        modeLabel->setText("Текущий режим: не определён");
    }
    updateWidgetDisplay();
}

void CustomDateWidget::updateWidgetDisplay()
{
    if (currentMode == DtMode || currentMode == UdtMode || currentMode == MudtMode) {
        lineEdit->hide();
        dateTimeEdit->show();
    } else {
        dateTimeEdit->hide();
        lineEdit->show();
    }
}

QString CustomDateWidget::getNumericValue()
{
    QVariant numericValue = convertDateValue(dateTimeEdit->dateTime());
    if (currentMode == DtMode) {
        return QString::number(numericValue.toLongLong());
    }
    else if (currentMode == UdtMode || currentMode == MudtMode) {
        return QString::number(numericValue.toDouble(), 'f', 8);
    }
    return QString();
}

void CustomDateWidget::setNumericValue(const QString &value)
{
    QDateTime dt;
    bool ok = false;
    if (currentMode == DtMode) {
        // Если пришло число с плавающей точкой, округляем его до ближайшего целого
        double raw = value.toDouble(&ok);
        if (ok) {
            qint64 seconds = qRound64(raw);
            dt = dtToQDateTime(seconds);
        }
    } else if (currentMode == UdtMode) {
        double jd = value.toDouble(&ok);
        if (ok)
            dt = julianToQDateTime(jd);
    } else if (currentMode == MudtMode) {
        double mjd = value.toDouble(&ok);
        if (ok)
            dt = modifiedJulianToQDateTime(mjd);
    }
    if (dt.isValid())
        dateTimeEdit->setDateTime(dt);
}

QVariant CustomDateWidget::convertDateValue(const QDateTime &dt)
{
    if (currentMode == DtMode) {
        // Если время равно ссылочному, возвращаем 0
        if (dt == refDateTime())
            return QVariant::fromValue(qint64(0));
        return QVariant::fromValue(QDateTimeToDt(dt));
    } else if (currentMode == UdtMode) {
        double jd = qDateTimeToJulian(dt);
        return QVariant::fromValue(jd);
    } else if (currentMode == MudtMode) {
        double mjd = qDateTimeToModifiedJulian(dt);
        return QVariant::fromValue(mjd);
    }
    return QVariant();
}

QDateTime CustomDateWidget::convertToQDateTime(const QVariant &value)
{
    if (!value.isValid())
        return QDateTime();
    if (currentMode == DtMode) {
        double d = value.toDouble();
        qint64 secs = qRound64(d);
        return dtToQDateTime(secs);
    } else if (currentMode == UdtMode) {
        double jd = value.toDouble();
        return julianToQDateTime(jd);
    } else if (currentMode == MudtMode) {
        double mjd = value.toDouble();
        return modifiedJulianToQDateTime(mjd);
    }
    return QDateTime();
}

// --- Функции для режима dt ---
QDateTime CustomDateWidget::dtToQDateTime(qint64 seconds)
{
    return refDateTime().addSecs(seconds);
}

qint64 CustomDateWidget::QDateTimeToDt(const QDateTime &dt)
{
    return refDateTime().secsTo(dt);
}

// --- Функции для юлианской даты ---
double CustomDateWidget::qDateTimeToJulian(const QDateTime &dt)
{
    // Если dt равен ссылочной дате, возвращаем 0.0
    if (dt == refDateTime())
        return 0.0;
    // Для локального времени: получаем миллисекунды от эпохи в локальном часовом поясе
    qint64 localMs = dt.toMSecsSinceEpoch();
    // Вычисляем jd по формуле: jd = (ms/86400000.0) + 2440587.5.
    double jd = (static_cast<double>(localMs) / 86400000.0) + 2440587.5;
    return jd;
}

QDateTime CustomDateWidget::julianToQDateTime(double jd)
{
    // Если jd равно 0 (или почти 0), возвращаем ссылочную дату
    if (qFuzzyIsNull(jd))
        return refDateTime();
    // Вычисляем локальное количество миллисекунд
    qint64 localMs = static_cast<qint64>(qRound64((jd - 2440587.5) * 86400000.0));
    // Создаём QDateTime в локальном часовом поясе
    return QDateTime::fromMSecsSinceEpoch(localMs, Qt::LocalTime);
}

double CustomDateWidget::qDateTimeToModifiedJulian(const QDateTime &dt)
{
    double jd = qDateTimeToJulian(dt);
    if (qFuzzyIsNull(jd))
        return 0.0;
    return jd - 2400000.5;
}

QDateTime CustomDateWidget::modifiedJulianToQDateTime(double mjd)
{
    if (qFuzzyIsNull(mjd))
        return refDateTime();
    double jd = mjd + 2400000.5;
    return julianToQDateTime(jd);
}
