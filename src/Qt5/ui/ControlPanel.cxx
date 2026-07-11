#include "ControlPanel.h"
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>

namespace vsapp {

ControlPanel::ControlPanel(QWidget *parent) : QWidget(parent) {
    setupUI();
    setupConnections();
}

void ControlPanel::setupUI() {
    playPauseBtn_ = new QPushButton("Play", this);
    progressSlider_ = new QSlider(Qt::Horizontal, this);
    timeLabel_ = new QLabel("00:00 / 00:00", this);

    progressSlider_->setRange(0, 100);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(playPauseBtn_);
    layout->addWidget(progressSlider_, 1);
    layout->addWidget(timeLabel_);

    setLayout(layout);
}

void ControlPanel::setupConnections() {
    connect(playPauseBtn_, &QPushButton::clicked, this, &ControlPanel::playPauseClicked);
    connect(progressSlider_, &QSlider::sliderMoved, this, &ControlPanel::seekRequested);
}

void ControlPanel::setPlayingState(bool isPlaying) {
    isPlaying_ = isPlaying;
    playPauseBtn_->setText(isPlaying_ ? "Pause" : "Play");
}

void ControlPanel::updateProgress(int currentValue, int maxValue) {
    progressSlider_->blockSignals(true);
    progressSlider_->setRange(0, maxValue);
    progressSlider_->setValue(currentValue);
    progressSlider_->blockSignals(false);
}

} // vsapp