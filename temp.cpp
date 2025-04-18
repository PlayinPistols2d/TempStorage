// RocketProgressBar.h
#ifndef ROCKETPROGRESSBAR_H
#define ROCKETPROGRESSBAR_H

#include <QWidget>
#include <QString>

class RocketProgressBar : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit RocketProgressBar(QWidget *parent = nullptr);

    int value() const { return m_value; }

public slots:
    void setValue(int v);

signals:
    void valueChanged(int);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString  m_earthEmoji;
    QString  m_rocketEmoji;
    QString  m_moonEmoji;
    int      m_value = 0;
};

#endif // ROCKETPROGRESSBAR_H




// RocketProgressBar.cpp
#include "RocketProgressBar.h"
#include <QPainter>
#include <QFontMetrics>

RocketProgressBar::RocketProgressBar(QWidget *parent)
  : QWidget(parent),
    m_earthEmoji("🌍"),
    m_rocketEmoji("🚀"),
    m_moonEmoji("🌕")
{
    // reserve enough height for the emoji
    setMinimumHeight(50);
}

void RocketProgressBar::setValue(int v) {
    v = qBound(0, v, 100);
    if (m_value == v) return;
    m_value = v;
    update();
    emit valueChanged(v);
}

void RocketProgressBar::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::TextAntialiasing);

    // 1) fill background with #31363b
    p.fillRect(rect(), QColor("#31363b"));

    const int H = height();
    const int W = width();

    // 2) pick a font size that fills ~80% of height
    QFont f = p.font();
    f.setPixelSize(int(H * 0.8));
    p.setFont(f);
    QFontMetrics fm(f);

    // helper: draw an emoji centered in [x,0,H,H]
    auto drawEmoji = [&](const QString &emoji, int x) {
        QRect r(x, 0, H, H);
        p.drawText(r, Qt::AlignCenter, emoji);
    };

    // draw Earth at left
    drawEmoji(m_earthEmoji, 0);

    // draw Moon at right (full moon emoji 🌕)
    drawEmoji(m_moonEmoji, W - H);

    // draw Rocket interpolated
    qreal t = m_value / 100.0;
    int   xPos = int((W - H) * t);
    drawEmoji(m_rocketEmoji, xPos);
}




// LoadingDialog.h
#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H

#include <QDialog>
class RocketProgressBar;

class LoadingDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoadingDialog(QWidget *parent = nullptr);

public slots:
    void setProgress(int percent);

private:
    RocketProgressBar* m_bar;
};

#endif // LOADINGDIALOG_H




// LoadingDialog.cpp
#include "LoadingDialog.h"
#include "RocketProgressBar.h"
#include <QVBoxLayout>

LoadingDialog::LoadingDialog(QWidget *parent)
  : QDialog(parent)
{
    // frameless so the bar covers full dialog
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setModal(true);

    m_bar = new RocketProgressBar(this);

    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->setSpacing(0);
    lay->addWidget(m_bar);
    setLayout(lay);

    // match width to something reasonable
    resize(400, m_bar->minimumHeight());
}

void LoadingDialog::setProgress(int percent) {
    m_bar->setValue(percent);
}




// Worker.h
#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QString>

class Worker : public QObject {
    Q_OBJECT

public:
    explicit Worker(const QString& query, QObject *parent = nullptr);

public slots:
    void process();

signals:
    void progress(int);
    void finished();
    void error(const QString& message);

private:
    QString m_query;

    bool execQuery(const QString& q) {
        // your real DB logic here; return false on failure
        Q_UNUSED(q);
        return true;
    }
};

#endif // WORKER_H



// Worker.cpp
#include "Worker.h"
#include <QThread>

Worker::Worker(const QString& query, QObject* parent)
  : QObject(parent), m_query(query)
{}

void Worker::process() {
    const int totalSteps = 100;
    for (int i = 0; i <= totalSteps; ++i) {
        if (!execQuery(m_query)) {
            emit error(tr("DB error at step %1").arg(i));
            return;
        }
        emit progress(i * 100 / totalSteps);
        QThread::msleep(20);  // simulate workload
    }
    emit finished();
}




// MainWindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Worker.h"
#include "LoadingDialog.h"

namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnImport_clicked();

private:
    Ui::MainWindow *ui;

    // assume you already have this in your existing code:
    void addLog(int type, const QString &msg);
};

#endif // MAINWINDOW_H



// MainWindow.cpp
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_btnImport_clicked() {
    // build your confirmation text exactly as before...
    QString prompt = tr("Удалить перед импортом из файла?");
    if (ui->cb_type->currentText() == "ЧК ВЧК")
        prompt = tr("Удалить из базы данных перед импортом из файла?");
    else if (ui->cb_type->currentText() == "МЦИ")
        prompt = tr("Удалить существующие %1 из базы данных и импортировать новые из файла?")
                     .arg(ui->cb_mciCmd->currentText());

    QMessageBox dlg(this);
    dlg.setWindowTitle(tr("Предупреждение"));
    dlg.setText(prompt);
    dlg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    dlg.button(QMessageBox::Ok)->setText(tr("ОК"));
    dlg.button(QMessageBox::Cancel)->setText(tr("Отмена"));
    if (dlg.exec() != QMessageBox::Ok)
        return;

    // assemble your real SQL or task into `query`
    QString query = /* ... */ "";

    // --- show loading dialog and run Worker in background ---
    LoadingDialog *loadDlg = new LoadingDialog(this);
    QThread        *thread  = new QThread(loadDlg);
    Worker         *worker  = new Worker(query);

    worker->moveToThread(thread);
    connect(thread,  &QThread::started,      worker, &Worker::process);
    connect(worker,  &Worker::progress,      loadDlg, &LoadingDialog::setProgress);
    connect(worker,  &Worker::finished,      loadDlg, &LoadingDialog::accept);
    connect(worker,  &Worker::error,         this,    [=](const QString &e){
        addLog(/*LogMessageType::critical*/2, e);
        loadDlg->reject();
    });
    connect(worker,  &Worker::finished,      thread,  &QThread::quit);
    connect(thread,  &QThread::finished,     worker,  &QObject::deleteLater);
    connect(thread,  &QThread::finished,     thread,  &QObject::deleteLater);

    thread->start();
    if (loadDlg->exec() == QDialog::Accepted)
        addLog(/*LogMessageType::common*/1, tr("Import finished successfully"));
}