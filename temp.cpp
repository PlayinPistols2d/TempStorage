// RocketLoadingScreen.h
#pragma once

#include <QDialog>

class QHBoxLayout;
class QLabel;
class QSpacerItem;

class RocketLoadingScreen : public QDialog {
    Q_OBJECT
    Q_PROPERTY(int progressValue READ progressValue WRITE setProgressValue)

public:
    explicit RocketLoadingScreen(QWidget* parent = nullptr);
    ~RocketLoadingScreen() override = default;

    /// Animate to this percent (0…100). Can be invoked from any thread.
    Q_SLOT void setProgress(int percent);

    /// For QPropertyAnimation
    int progressValue() const;
    Q_SLOT void setProgressValue(int percent);

private:
    QWidget*      m_barWidget;
    QHBoxLayout*  m_layout;
    QLabel*       m_earthLabel;
    QLabel*       m_rocketLabel;
    QLabel*       m_moonLabel;
    QSpacerItem*  m_leftSpacer;
    QSpacerItem*  m_rightSpacer;

    int           m_progress = 0;
};





// RocketLoadingScreen.cpp
#include "RocketLoadingScreen.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QPropertyAnimation>
#include <QApplication>
#include <Qt>

RocketLoadingScreen::RocketLoadingScreen(QWidget* parent)
  : QDialog(parent)
{
    // ——— Overlay setup ———
    setWindowFlags(windowFlags() |
                   Qt::FramelessWindowHint |
                   Qt::Dialog);
    setModal(true);
    // allow semi‑transparent background
    setAttribute(Qt::WA_TranslucentBackground);
    // blue tint at 50% opacity
    setStyleSheet("background-color: rgba(0, 122, 204, 128);");

    // make full‑size over parent
    if (parent) {
        resize(parent->size());
        move(parent->pos());
    }

    // ——— Bar widget ———
    m_barWidget = new QWidget(this);
    m_barWidget->setFixedHeight(60);
    m_barWidget->setStyleSheet(
        "background-color: #31363b;"
        "border-radius: 10px;"
    );

    // center the bar in the overlay
    auto *mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(0,0,0,0);
    mainLay->addStretch();
    mainLay->addWidget(m_barWidget, 0, Qt::AlignHCenter);
    mainLay->addStretch();

    // ——— Emoji layout inside the bar ———
    m_layout = new QHBoxLayout(m_barWidget);
    m_layout->setContentsMargins(15, 0, 15, 0);
    m_layout->setSpacing(0);

    m_earthLabel  = new QLabel("🌍", m_barWidget);
    m_rocketLabel = new QLabel("🚀", m_barWidget);
    m_moonLabel   = new QLabel("🌕", m_barWidget);
    for (auto *lbl : {m_earthLabel, m_rocketLabel, m_moonLabel})
        lbl->setAlignment(Qt::AlignCenter);

    m_leftSpacer  = new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_rightSpacer = new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_layout->addWidget(m_earthLabel);
    m_layout->addItem(m_leftSpacer);
    m_layout->addWidget(m_rocketLabel);
    m_layout->addItem(m_rightSpacer);
    m_layout->addWidget(m_moonLabel);

    // start hidden; show() & setProgress(0) when you’re ready
}

int RocketLoadingScreen::progressValue() const {
    return m_progress;
}

void RocketLoadingScreen::setProgressValue(int percent) {
    // internal setter for animation
    m_progress = percent;
    m_layout->setStretch(1, m_progress);
    m_layout->setStretch(3, 100 - m_progress);
    // force repaint
    QApplication::processEvents();
}

void RocketLoadingScreen::setProgress(int percent) {
    percent = qBound(0, percent, 100);

    // animate from current → new
    auto *anim = new QPropertyAnimation(this, "progressValue");
    anim->setDuration(400);
    anim->setStartValue(m_progress);
    anim->setEndValue(percent);
    anim->setEasingCurve(QEasingCurve::InOutCubic);
    connect(anim, &QPropertyAnimation::finished, anim, &QObject::deleteLater);
    anim->start();
}





// in your function:
auto* loader = new RocketLoadingScreen(this);
loader->show();
loader->setProgress(0);

// … your existing if/DB calls …  
// after each step, bump the loader:
QMetaObject::invokeMethod(loader,
                          "setProgress",
                          Qt::QueuedConnection,
                          Q_ARG(int, 30));
// … more work …
QMetaObject::invokeMethod(loader,
                          "setProgress",
                          Qt::QueuedConnection,
                          Q_ARG(int, 60));
// … final import …
QMetaObject::invokeMethod(loader,
                          "setProgress",
                          Qt::QueuedConnection,
                          Q_ARG(int, 100));

// when done or on error:
QMetaObject::invokeMethod(loader,
                          [loader](){
                            loader->hide();
                            loader->deleteLater();
                          },
                          Qt::QueuedConnection);

