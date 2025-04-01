#include "RainbowButton.h"

EmojiParticle::EmojiParticle(const QString &emoji, QWidget *parent)
    : QLabel(parent), gravity(0.6) {
    setText(emoji);
    setStyleSheet("font-size: 20px;");
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFixedSize(30, 30);

    // Randomized initial velocity
    vx = QRandomGenerator::global()->bounded(-3, 4);  // Small horizontal movement
    vy = QRandomGenerator::global()->bounded(-15, -8); // Shoot upwards

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &EmojiParticle::updatePosition);
    timer->start(30);
}

void EmojiParticle::updatePosition() {
    move(x() + vx, y() + vy);
    vy += gravity; // Gravity effect

    // Check if it reached the bottom of the window
    if (y() > parentWidget()->height()) {
        deleteLater(); // Remove emoji once it reaches the bottom
    }
}



void RainbowButton::spawnEmojis() {
    static QStringList emojis = { "🎉", "🥳", "✨", "🔥", "💖", "🎊", "😃" };

    for (int i = 0; i < 10; ++i) {
        auto *emoji = new EmojiParticle(emojis[QRandomGenerator::global()->bounded(emojis.size())], parentWidget());
        emoji->move(mapToParent(rect().center())); // Start at button center
        emoji->show();
    }
}

