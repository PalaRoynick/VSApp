#pragma once

#include "ControlPanel.h"
#include "VideoWidget.h"
#include "../player/MediaPlayer.h"

#include <QMainWindow>

namespace vsapp {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    VideoWidget  *videoWidget_;
    ControlPanel *controlPanel_;
    MediaPlayer  *player_;
    
    void setupUI();
    void setupConnections();
};

} // vsapp