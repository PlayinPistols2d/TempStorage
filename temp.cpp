// RocketLoadingScreen.h
#pragma once

#include <QDialog>

class QHBoxLayout;
class QLabel;
class QSpacerItem;

class RocketLoadingScreen : public QDialog {
    Q_OBJECT
public:
    explicit RocketLoadingScreen(QWidget* parent = nullptr);

    /// percent from 0…100; call this after each DB step
    void setProgress(int percent);

private:
    QHBoxLayout*  m_layout;
    QLabel*       m_earthLabel;
    QLabel*       m_rocketLabel;
    QLabel*       m_moonLabel;
    QSpacerItem*  m_leftSpacer;
    QSpacerItem*  m_rightSpacer;
};






// RocketLoadingScreen.cpp
#include "RocketLoadingScreen.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>
#include <QApplication>
#include <Qt>

RocketLoadingScreen::RocketLoadingScreen(QWidget* parent)
  : QDialog(parent)
{
    // frameless, non‑modal overlay
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Dialog);
    setModal(false);

    // dark #31363b background
    setStyleSheet("background-color: #31363b;");

    // fixed height (width will adjust to parent/layout)
    setFixedHeight(50);

    // layout:  🌍 | ◼︎Spacer◼︎ | 🚀 | ◼︎Spacer◼︎ | 🌕
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(0);

    m_earthLabel  = new QLabel("🌍", this);
    m_rocketLabel = new QLabel("🚀", this);
    m_moonLabel   = new QLabel("🌕", this);
    for (QLabel* lbl : {m_earthLabel, m_rocketLabel, m_moonLabel})
        lbl->setAlignment(Qt::AlignCenter);

    m_leftSpacer  = new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_rightSpacer = new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_layout->addWidget(m_earthLabel);
    m_layout->addItem(m_leftSpacer);
    m_layout->addWidget(m_rocketLabel);
    m_layout->addItem(m_rightSpacer);
    m_layout->addWidget(m_moonLabel);
}

void RocketLoadingScreen::setProgress(int percent)
{
    percent = qBound(0, percent, 100);
    // spacer at index 1 is left, at index 3 is right
    m_layout->setStretch(1, percent);
    m_layout->setStretch(3, 100 - percent);
    // force immediate repaint even if DB call is blocking
    QApplication::processEvents();
}



 #include "RocketLoadingScreen.h"

void MyWindow::yourImportFunction()
{
    // 1) create & show the loader
    auto* loader = new RocketLoadingScreen(this);
    loader->show();
    loader->setProgress(0);

    // 2) your unchanged if/DB logic, just add setProgress() and cleanup
    if (ui->cb_type->currentText() == "text") {
        // … your QMessageBox code …

        loader->setProgress(20);
        bool ok = worker->execQuery(query);
        if (!ok) {
            loader->hide();
            loader->deleteLater();
            addLog(LogMessageType::critical, "Ошибка удаления из базы данных");
            return;
        }

        loader->setProgress(50);
        addLog(LogMessageType::common, "успешно удалены из базы данных");
    }
    else if (ui->cb_type->currentText() == "ЧК ВЧК") {
        // … same pattern …
        loader->setProgress(20);
        worker->execQuery(query);
        // … check ok …
        loader->setProgress(40);
        worker->execQuery(query);
        // … check ok …
        loader->setProgress(60);
    }
    else if (ui->cb_type->currentText() == "МЦИ") {
        // … same pattern …
        loader->setProgress(20);
        worker->execQuery(query);
        // … check ok …
        loader->setProgress(40);
        worker->execQuery(query);
        // … check ok …
        loader->setProgress(60);
    }

    // final import step
    importToDb(cur);
    loader->setProgress(100);

    // 3) teardown
    loader->hide();
    loader->deleteLater();
}