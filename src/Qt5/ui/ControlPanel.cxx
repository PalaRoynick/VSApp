#include "ControlPanel.h"
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>

#include <QDebug>

namespace vsapp {

ControlPanel::ControlPanel(QWidget *parent) : QWidget(parent) {
    setupUI();
    setupConnections();
}

void ControlPanel::setupUI() {
    playPauseBtn_ = new QPushButton("Play", this);
    progressSlider_ = new QSlider(Qt::Horizontal, this);
    timeLabel_ = new QLabel("00:00 / 00:00", this);

    progressSlider_->setRange(0, 0);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(playPauseBtn_);
    layout->addWidget(progressSlider_, 1);
    layout->addWidget(timeLabel_);

    setLayout(layout);
}

void ControlPanel::setDuration(int durationMs) {
    qDebug() << "setDuration called with:" << durationMs << "ms";

    assert(durationMs >= 0);
    progressSlider_->setRange(0, durationMs);

    int totalSeconds = durationMs / 1000;
    int hours        = totalSeconds / 3600;

    if (hours == 0)
        timeLabel_->setText(QString("00:00 / %1").arg(formatTime(durationMs)));
    else
        timeLabel_->setText(QString("0:00:00 / %1").arg(formatTime(durationMs)));
}

void ControlPanel::setupConnections() {
    connect(playPauseBtn_, &QPushButton::clicked, this, &ControlPanel::playPauseClicked);
    connect(progressSlider_, &QSlider::sliderMoved, this, &ControlPanel::seekRequested);
}

void ControlPanel::setPlayingState(bool isPlaying) {
    isPlaying_ = isPlaying;
    playPauseBtn_->setText(isPlaying_ ? "Pause" : "Play");
}

void ControlPanel::updateProgress(int currentValueMs, int maxValueMs) {
    Q_UNUSED(maxValueMs);

    progressSlider_->blockSignals(true);
    progressSlider_->setValue(currentValueMs);
    progressSlider_->blockSignals(false);

    QString currentTime = formatTime(currentValueMs);
    timeLabel_->setText(currentTime + " / " + formatTime(progressSlider_->maximum()));
}

QString ControlPanel::formatTime(int milliseconds) const {
    if (milliseconds < 0) {
        milliseconds = 0;
    }

    int totalSeconds = milliseconds / 1000;
    int hours        = totalSeconds / 3600;
    int minutes      = (totalSeconds % 3600) / 60;
    int seconds      = totalSeconds % 60;

    if (hours > 0)
        return QString("%1:%2:%3")
        .arg(hours,   2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));

    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

} // vsapp