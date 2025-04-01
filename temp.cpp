#ifndef RAINBOWBUTTON_H
#define RAINBOWBUTTON_H

#include <QPushButton>
#include <QPainter>
#include <QLinearGradient>
#include <QTimer>
#include <QRandomGenerator>
#include <QLabel>

class EmojiParticle : public QLabel {
    Q_OBJECT
public:
    explicit EmojiParticle(const QString &emoji, QWidget *parent = nullptr);

private slots:
    void updatePosition();

private:
    int vx, vy;
    float gravity;
    QTimer *timer;
};

class RainbowButton : public QPushButton {
    Q_OBJECT

public:
    explicit RainbowButton(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private:
    int m_offset;
    bool m_pressed;
    bool m_hovered;

    QColor gradientColorAt(double position);
    void spawnEmojis();
};

#endif // RAINBOWBUTTON_H




#include "RainbowButton.h"

EmojiParticle::EmojiParticle(const QString &emoji, QWidget *parent)
    : QLabel(parent), gravity(0.5) {
    setText(emoji);
    setStyleSheet("font-size: 20px;");
    setAttribute(Qt::WA_TransparentForMouseEvents);
    move(parent->width() / 2, parent->height() / 2);
    setFixedSize(30, 30);

    vx = QRandomGenerator::global()->bounded(-5, 6);  // Random horizontal velocity
    vy = QRandomGenerator::global()->bounded(-10, -5); // Random upward velocity

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &EmojiParticle::updatePosition);
    timer->start(30);
}

void EmojiParticle::updatePosition() {
    move(x() + vx, y() + vy);
    vy += gravity; // Simulate gravity effect
    if (y() > parentWidget()->height()) delete this; // Remove once off-screen
}

RainbowButton::RainbowButton(QWidget *parent) 
    : QPushButton(parent), m_offset(0), m_pressed(false), m_hovered(false) {
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    startTimer(50); // Updates button color shift
}

void RainbowButton::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Create rainbow gradient
    QLinearGradient gradient(0, 0, width(), 0);
    const int steps = 8;
    for (int i = 0; i <= steps; ++i) {
        float pos = static_cast<float>(i) / steps;
        int hue = (m_offset + static_cast<int>(pos * 360)) % 360;
        QColor color = QColor::fromHsv(hue, 255, 255);
        if (m_pressed) color = color.darker(120);
        else if (m_hovered) color = color.lighter(110);
        gradient.setColorAt(pos, color);
    }

    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    QRectF btnRect = m_pressed ? rect().adjusted(2, 2, -2, -2) : rect();
    painter.drawRoundedRect(btnRect, 10, 10);

    // Determine best text color for contrast
    QColor avgColor = gradientColorAt(0.5);
    double luminance = (0.299 * avgColor.red() + 0.587 * avgColor.green() + 0.114 * avgColor.blue()) / 255;
    QColor textColor = luminance > 0.5 ? Qt::black : Qt::white;

    // Draw text
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    painter.setPen(textColor);
    painter.drawText(btnRect.toRect(), Qt::AlignCenter, text());
}

void RainbowButton::enterEvent(QEvent *event) {
    QPushButton::enterEvent(event);
    m_hovered = true;
    update();
}

void RainbowButton::leaveEvent(QEvent *event) {
    QPushButton::leaveEvent(event);
    m_hovered = false;
    update();
}

void RainbowButton::mousePressEvent(QMouseEvent *event) {
    QPushButton::mousePressEvent(event);
    m_pressed = true;
    spawnEmojis();
    update();
}

void RainbowButton::mouseReleaseEvent(QMouseEvent *event) {
    QPushButton::mouseReleaseEvent(event);
    m_pressed = false;
    update();
}

void RainbowButton::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event);
    m_offset = (m_offset + 3) % 360;
    update();
}

QColor RainbowButton::gradientColorAt(double position) {
    int hue = (m_offset + static_cast<int>(position * 360)) % 360;
    return QColor::fromHsv(hue, 255, 255);
}

void RainbowButton::spawnEmojis() {
    static QStringList emojis = { "🎉", "🥳", "✨", "🔥", "💖", "🎊", "😃" };
    for (int i = 0; i < 10; ++i) {
        auto *emoji = new EmojiParticle(emojis[QRandomGenerator::global()->bounded(emojis.size())], parentWidget());
        emoji->move(mapToParent(rect().center())); // Start at button center
        emoji->show();
    }
}


