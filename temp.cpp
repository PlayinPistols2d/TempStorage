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

    /// Thread‑safe entry point: animate rocket to `percent` (0…100)
    Q_SLOT void setProgress(int percent);

    // used by the animation
    int progressValue() const;
    Q_SLOT void setProgressValue(int percent);

protected:
    void showEvent(QShowEvent* ev) override;

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
#include <QShowEvent>
#include <Qt>

RocketLoadingScreen::RocketLoadingScreen(QWidget* parent)
  : QDialog(parent)
{
    setWindowFlags(windowFlags() |
                   Qt::FramelessWindowHint |
                   Qt::Dialog);
    setModal(true);
    setAttribute(Qt::WA_TranslucentBackground);

    // semi‑transparent blue overlay
    setStyleSheet("background-color: rgba(0, 122, 204, 128);");

    // center the bar vertically
    auto *mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(0,0,0,0);
    mainLay->addStretch();

    // the bar container
    m_barWidget = new QWidget(this);
    m_barWidget->setStyleSheet(
        "background-color: #31363b;"
        "border-radius: 10px;"
    );

    mainLay->addWidget(m_barWidget, 0, Qt::AlignHCenter);
    mainLay->addStretch();

    // inside the bar: 🌍 – 🚀 – 🌕
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
}

void RocketLoadingScreen::showEvent(QShowEvent* ev)
{
    QDialog::showEvent(ev);
    if (parentWidget()) {
        // 90% of parent width, 100px tall
        int barW = int(parentWidget()->width() * 0.9);
        m_barWidget->setFixedSize(barW, 100);

        // scale emoji fonts to ~80px
        QFont f = m_earthLabel->font();
        f.setPixelSize(int(100 * 0.8));
        for (auto *lbl : {m_earthLabel, m_rocketLabel, m_moonLabel})
            lbl->setFont(f);
    }
    // ensure initial position
    setProgressValue(m_progress);
}

int RocketLoadingScreen::progressValue() const {
    return m_progress;
}

void RocketLoadingScreen::setProgressValue(int percent) {
    m_progress = percent;
    m_layout->setStretch(1, m_progress);
    m_layout->setStretch(3, 100 - m_progress);
    QApplication::processEvents();
}

void RocketLoadingScreen::setProgress(int percent) {
    percent = qBound(0, percent, 100);
    auto *anim = new QPropertyAnimation(this, "progressValue");
    anim->setDuration(600);
    anim->setStartValue(m_progress);
    anim->setEndValue(percent);
    anim->setEasingCurve(QEasingCurve::InOutCubic);
    connect(anim, &QPropertyAnimation::finished, anim, &QObject::deleteLater);
    anim->start();
}
