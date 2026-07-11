#pragma once

#include <QWidget>

class QPushButton;
class QSlider;
class QLabel;

namespace vsapp {

class ControlPanel : public QWidget {
    Q_OBJECT
public:
    explicit ControlPanel(QWidget *parent = nullptr);
    ~ControlPanel() override = default;

public slots:
    void setPlayingState(bool isPlaying);
    void updateProgress(int currentValue, int maxValue);

signals:
    void playPauseClicked();
    void seekRequested(int position);

private:
    QPushButton* playPauseBtn_ = nullptr;
    QSlider* progressSlider_ = nullptr;
    QLabel*  timeLabel_ = nullptr;

    bool isPlaying_ = false;

    void setupUI();
    void setupConnections();
};

} // vsapp