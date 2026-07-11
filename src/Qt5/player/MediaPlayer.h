#pragma once
#include <QObject>
#include <QImage>
#include <QTimer>
#include <memory>

namespace vsapp {

class MediaDecoder;
class FrameConverter;

class MediaPlayer : public QObject {
    Q_OBJECT
public:
    explicit MediaPlayer(QObject *parent = nullptr);
    ~MediaPlayer();

    void loadFile(const QString &filePath);
    void play();
    void pause();
    bool isPlaying() const;

public slots:
    void seekTo(int positionMs);

signals:
    void frameReady(const QImage &frame);
    void playbackFinished();
    void positionChanged(int currentPositionMs, int durationMs);

private slots:
    void decodingLoop();

private:
    MediaDecoder *decoder_;
    FrameConverter *converter_;
    QTimer *decodeTimer_ = nullptr;
    bool isPlaying_ = false;
    int durationMs_ = 0;
};

} // vsapp