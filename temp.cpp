#include <QDateEdit>
#include <QString>
#include <QDateTime>

class CustomDateWidget : public QDateEdit {
    Q_OBJECT

public:
    // Перечисление для режимов
    enum class Mode {
        DT,
        UDT,
        MUDT
    };

    explicit CustomDateWidget(QWidget *parent = nullptr);

    // Основные методы
    void setMode(const QString &modeStr);
    void setNumericValue(const QString &valueStr);
    QString getNumericValue() const;

private:
    Mode currentMode = Mode::DT; // Режим по умолчанию — DT

    // Вспомогательная функция для получения текущей даты в UTC
    QDateTime currentDateTimeUtc() const {
        QDate date = this->date();
        return QDateTime(date, QTime(0, 0, 0), Qt::UTC);
    }

    // Функции преобразования
    qint64 dateToDt(const QDateTime &dateTime) const;
    QDateTime dtToDate(qint64 dt) const;
    double dateToUdt(const QDateTime &dateTime) const;
    QDateTime udtToDate(double udt) const;
    double dateToMudt(const QDateTime &dateTime) const;
    QDateTime mudtToDate(double mudt) const;
};




#include <QDateEdit>
#include <QString>
#include <QDateTime>

class CustomDateWidget : public QDateEdit {
    Q_OBJECT

public:
    enum class Mode {
        DT,
        UDT,
        MUDT
    };

    explicit CustomDateWidget(QWidget *parent = nullptr);

    void setMode(const QString &modeStr);
    void setNumericValue(const QString &valueStr);
    QString getNumericValue() const;

private:
    Mode currentMode = Mode::DT;

    QDateTime currentDateTimeUtc() const {
        QDate date = this->date();
        return QDateTime(date, QTime(0, 0, 0), Qt::UTC);
    }

    qint64 dateToDt(const QDateTime &dateTime) const;
    QDateTime dtToDate(qint64 dt) const;
    double dateToUdt(const QDateTime &dateTime) const;
    QDateTime udtToDate(double udt) const;
    double dateToMudt(const QDateTime &dateTime) const;
    QDateTime mudtToDate(double mudt) const;
};

CustomDateWidget::CustomDateWidget(QWidget *parent)
    : QDateEdit(parent) {
}

void CustomDateWidget::setMode(const QString &modeStr) {
    QString lowerMode = modeStr.toLower();
    if (lowerMode == "dt") {
        currentMode = Mode::DT;
    } else if (lowerMode == "udt") {
        currentMode = Mode::UDT;
    } else if (lowerMode == "mudt") {
        currentMode = Mode::MUDT;
    }
}

void CustomDateWidget::setNumericValue(const QString &valueStr) {
    QDateTime dt;
    if (currentMode == Mode::DT) {
        bool ok;
        qint64 dtValue = valueStr.toLongLong(&ok);
        if (ok) {
            dt = dtToDate(dtValue);
        }
    } else if (currentMode == Mode::UDT) {
        bool ok;
        double udtValue = valueStr.toDouble(&ok);
        if (ok) {
            dt = udtToDate(udtValue);
        }
    } else if (currentMode == Mode::MUDT) {
        bool ok;
        double mudtValue = valueStr.toDouble(&ok);
        if (ok) {
            dt = mudtToDate(mudtValue);
        }
    }
    if (dt.isValid()) {
        this->setDate(dt.date());
    }
}

QString CustomDateWidget::getNumericValue() const {
    QDateTime dtUtc = currentDateTimeUtc();
    if (currentMode == Mode::DT) {
        qint64 dtValue = dateToDt(dtUtc);
        return QString::number(dtValue);
    } else if (currentMode == Mode::UDT) {
        double udtValue = dateToUdt(dtUtc);
        return QString::number(udtValue, 'f', 10);
    } else if (currentMode == Mode::MUDT) {
        double mudtValue = dateToMudt(dtUtc);
        return QString::number(mudtValue, 'f', 10);
    }
    return QString();
}

qint64 CustomDateWidget::dateToDt(const QDateTime &dateTime) const {
    QDateTime reference(QDate(2000, 1, 1), QTime(0, 0, 0), Qt::UTC);
    return reference.secsTo(dateTime);
}

QDateTime CustomDateWidget::dtToDate(qint64 dt) const {
    QDateTime reference(QDate(2000, 1, 1), QTime(0, 0, 0), Qt::UTC);
    return reference.addSecs(dt);
}

double CustomDateWidget::dateToUdt(const QDateTime &dateTime) const {
    qint64 unixTime = dateTime.toSecsSinceEpoch();
    double jd = 2440587.5 + unixTime / 86400.0;
    return jd;
}

QDateTime CustomDateWidget::udtToDate(double udt) const {
    double unixTimeDays = udt - 2440587.5;
    qint64 unixTime = static_cast<qint64>(unixTimeDays * 86400.0);
    return QDateTime::fromSecsSinceEpoch(unixTime, Qt::UTC);
}

double CustomDateWidget::dateToMudt(const QDateTime &dateTime) const {
    double jd = dateToUdt(dateTime);
    return jd - 2400000.5;
}

QDateTime CustomDateWidget::mudtToDate(double mudt) const {
    double jd = mudt + 2400000.5;
    return udtToDate(jd);
}