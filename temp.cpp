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
    bool grounded; // Added to track if particle has landed
};

class RainbowButton : public QPushButton {
    // Unchanged, omitted for brevity
};

#endif // RAINBOWBUTTON_H



#include "RainbowButton.h"

EmojiParticle::EmojiParticle(const QString &emoji, QWidget *parent)
    : QLabel(parent), gravity(0.6), grounded(false) {
    setText(emoji);
    setStyleSheet("font-size: 20px;");
    setAttribute(Qt::WA_TransparentForMouseEvents);
    move(parent->width() / 2, parent->height() / 2);
    setFixedSize(30, 30);

    // Adjusted for volcano effect
    vx = QRandomGenerator::global()->bounded(-5, 6);  // Wider horizontal spread
    vy = QRandomGenerator::global()->bounded(-20, -10); // Shoot higher upwards

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &EmojiParticle::updatePosition);
    timer->start(30);

    // Removed: QTimer::singleShot(3000, this, &QWidget::deleteLater);
}

void EmojiParticle::updatePosition() {
    if (!grounded) {
        int newX = x() + vx;
        int newY = y() + vy;
        vy += gravity; // Apply gravity

        // Check if it reaches the bottom
        if (newY >= parentWidget()->height() - height()) {
            newY = parentWidget()->height() - height(); // Snap to bottom
            vy = 0; // Stop vertical movement
            grounded = true; // Mark as landed
        }
        move(newX, newY);
    } else {
        // Grounded: handle horizontal sliding with friction
        int newX = x() + vx;

        // Keep within horizontal bounds
        if (newX < 0) {
            newX = 0;
            vx = 0;
        } else if (newX > parentWidget()->width() - width()) {
            newX = parentWidget()->width() - width();
            vx = 0;
        } else {
            vx *= 0.9; // Apply friction (reduce by 10% each frame)
        }
        move(newX, y());

        // If horizontal speed is negligible, stop and schedule deletion
        if (qAbs(vx) < 0.1) {
            timer->stop(); // Stop movement updates
            QTimer::singleShot(2000, this, &QWidget::deleteLater); // Delete after 2s rest
        }
    }
}