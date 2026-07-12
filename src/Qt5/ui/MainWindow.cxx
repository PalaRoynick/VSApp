#include "MainWindow.h"
#include "VideoWidget.h"
#include "ControlPanel.h"
#include "../player/MediaPlayer.h"

#include <QApplication>
#include <QVBoxLayout>
#include <QDebug>

namespace vsapp {

MainWindow::MainWindow(QWidget *parent) 
    : QMainWindow(parent) {
    player_ = new MediaPlayer(this);
    QVariant startupFile = qApp->property("startupFile");
    if (startupFile.isValid() && !startupFile.toString().isEmpty()) {
        player_->loadFile(startupFile.toString());
    }

    videoWidget_  = new VideoWidget(this);
    controlPanel_ = new ControlPanel(this);

    setupUI();
    setupConnections();

    setWindowTitle("VSApp Minimal Video Player");
    resize(800, 600);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(0);

    mainLayout->addWidget(videoWidget_, 1);
    mainLayout->addWidget(controlPanel_);

    setCentralWidget(centralWidget);
}

void MainWindow::setupConnections() {
    connect(controlPanel_, &ControlPanel::playPauseClicked, this, [this]() {
        if (player_->isPlaying()) {
            player_->pause();
        } else {
            player_->play();
        }
        controlPanel_->setPlayingState(player_->isPlaying());
    });

    connect(player_, &MediaPlayer::frameReady,
            videoWidget_, &VideoWidget::displayFrame);

    connect(player_, &MediaPlayer::positionChanged, 
            controlPanel_, &ControlPanel::updateProgress);

    connect(controlPanel_, &ControlPanel::seekRequested,
            player_, &MediaPlayer::seekTo);

    connect(player_, &MediaPlayer::playbackFinished, this, [this]() {
        controlPanel_->setPlayingState(false);
    });
}

} // vsapp