#include "statuscircle.h"
#include <QPainter>
#include <QPaintEvent>
#include <QFontMetrics>
#include <algorithm>

StatusCircle::StatusCircle(QWidget *parent)
    : QWidget(parent),
      m_state(false),
      m_animationValue(0.0),
      m_circleDiameter(20),
      m_colorPositive(Qt::green),
      m_colorNegative(Qt::red),
      m_textPositive("Connected"),
      m_textNegative("Disconnected"),
      m_animation(new QPropertyAnimation(this, "animationValue", this))
{
    // Длительность анимации (мс)
    m_animation->setDuration(300);
    m_animation->setStartValue(0.0);
    m_animation->setEndValue(1.0);

    // Задаем небольшие отступы, чтобы содержимое не слипалось с краями
    setContentsMargins(5,5,5,5);
}

void StatusCircle::setCircleDiameter(int diameter)
{
    m_circleDiameter = diameter;
    update();
}

int StatusCircle::circleDiameter() const
{
    return m_circleDiameter;
}

void StatusCircle::setPositiveColor(const QColor &color)
{
    m_colorPositive = color;
    update();
}

QColor StatusCircle::positiveColor() const
{
    return m_colorPositive;
}

void StatusCircle::setNegativeColor(const QColor &color)
{
    m_colorNegative = color;
    update();
}

QColor StatusCircle::negativeColor() const
{
    return m_colorNegative;
}

void StatusCircle::setPositiveText(const QString &text)
{
    m_textPositive = text;
    update();
}

QString StatusCircle::positiveText() const
{
    return m_textPositive;
}

void StatusCircle::setNegativeText(const QString &text)
{
    m_textNegative = text;
    update();
}

QString StatusCircle::negativeText() const
{
    return m_textNegative;
}

void StatusCircle::setState(bool state)
{
    if (m_state == state)
        return;

    m_state = state;

    // Целевое значение анимации: true → 1.0, false → 0.0
    qreal targetValue = state ? 1.0 : 0.0;
    m_animation->stop();
    m_animation->setStartValue(m_animationValue);
    m_animation->setEndValue(targetValue);
    m_animation->start();
}

bool StatusCircle::state() const
{
    return m_state;
}

qreal StatusCircle::animationValue() const
{
    return m_animationValue;
}

void StatusCircle::setAnimationValue(qreal value)
{
    m_animationValue = value;
    update();
}

QSize StatusCircle::sizeHint() const
{
    QFontMetrics fm(font());
    // Чтобы текст полностью умещался, считаем ширину на основе наибольшей длины текста
    int textWidth = std::max(fm.horizontalAdvance(m_textPositive), fm.horizontalAdvance(m_textNegative));
    int margin = 10; // внутренние отступы
    int preferredWidth = textWidth + margin + m_circleDiameter + margin;
    int preferredHeight = std::max(m_circleDiameter, fm.height()) + 2 * margin;
    return QSize(preferredWidth, preferredHeight);
}

void StatusCircle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Выбираем текст для отображения
    QString displayText = (m_animationValue >= 0.5) ? m_textPositive : m_textNegative;

    // Интерполяция цвета
    int r = static_cast<int>(m_colorNegative.red()   + (m_colorPositive.red()   - m_colorNegative.red()) * m_animationValue);
    int g = static_cast<int>(m_colorNegative.green() + (m_colorPositive.green() - m_colorNegative.green()) * m_animationValue);
    int b = static_cast<int>(m_colorNegative.blue()  + (m_colorPositive.blue()  - m_colorNegative.blue()) * m_animationValue);
    QColor currentColor(r, g, b);

    QFontMetrics fm(font());
    int textWidth = fm.horizontalAdvance(displayText);
    int textHeight = fm.height();

    int margin = 10;
    int textX = margin;
    int textY = (height() - textHeight) / 2 + fm.ascent();

    // Рисуем текст слева
    painter.drawText(textX, textY, displayText);

    // Расчёт позиции для круга: справа от текста
    int circleX = textX + textWidth + margin;
    int circleY = (height() - m_circleDiameter) / 2;
    QRect circleRect(circleX, circleY, m_circleDiameter, m_circleDiameter);

    painter.setPen(Qt::black);
    painter.setBrush(currentColor);
    painter.drawEllipse(circleRect);
}



#ifndef STATUSCIRCLE_H
#define STATUSCIRCLE_H

#include <QWidget>
#include <QColor>
#include <QString>
#include <QPropertyAnimation>

class StatusCircle : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal animationValue READ animationValue WRITE setAnimationValue)

public:
    explicit StatusCircle(QWidget *parent = nullptr);

    // Методы для настройки внешнего вида
    void setCircleDiameter(int diameter);
    int circleDiameter() const;

    void setPositiveColor(const QColor &color);
    QColor positiveColor() const;

    void setNegativeColor(const QColor &color);
    QColor negativeColor() const;

    void setPositiveText(const QString &text);
    QString positiveText() const;

    void setNegativeText(const QString &text);
    QString negativeText() const;

    // Переключение состояния (true – положительное, false – отрицательное)
    void setState(bool state);
    bool state() const;

    // Свойство для анимации перехода (0.0 - отрицательное, 1.0 - положительное)
    qreal animationValue() const;
    void setAnimationValue(qreal value);

    // Рекомендуемый размер виджета для корректного отображения текста и круга
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool m_state;                // Текущее состояние
    qreal m_animationValue;      // Значение для анимации
    int m_circleDiameter;        // Диаметр круга
    QColor m_colorPositive;      // Цвет положительного состояния
    QColor m_colorNegative;      // Цвет отрицательного состояния
    QString m_textPositive;      // Текст положительного состояния
    QString m_textNegative;      // Текст отрицательного состояния

    QPropertyAnimation *m_animation; // Анимация изменения animationValue
};

#endif // STATUSCIRCLE_H
