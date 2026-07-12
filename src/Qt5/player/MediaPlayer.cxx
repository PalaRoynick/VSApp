#include "MediaPlayer.h"
#include "../../MediaDecoder.h"
#include "../../FrameConverter.h"
#include <QTimer>
#include <QDebug>

extern "C" {
#include <libavutil/frame.h>
}

namespace vsapp {

MediaPlayer::MediaPlayer(QObject *parent) 
    : QObject(parent) 
{
    decoder_ = new MediaDecoder();
    converter_ = new FrameConverter();

    decodeTimer_ = new QTimer(this);
    connect(decodeTimer_, &QTimer::timeout, this, &MediaPlayer::decodingLoop);
}

MediaPlayer::~MediaPlayer() {
    pause();
    delete decoder_;
    delete converter_;
}

void MediaPlayer::loadFile(const QString &filePath, bool webCam) {
    pause();

    webCam_ = webCam;
    if (decoder_->openFile(filePath.toStdString(), webCam_)) {
        qDebug() << "File loaded successfully. Resolution:" 
                 << decoder_->getWidth() << "x" << decoder_->getHeight();

        if (!webCam_) {
            durationMs_ = decoder_->getDuration();
            emit durationChanged(durationMs_);
        }

        double fps = decoder_->getFps();
        if (fps > 0) {
            int intervalMs = static_cast<int>(1000.0 / fps);
            decodeTimer_->setInterval(intervalMs);
        } else {
            decodeTimer_->setInterval(33);
        }
    } else {
        qWarning() << "Failed to open file:" << filePath;
    }
}

void MediaPlayer::play() {
    if (decoder_->isOpened() && !isPlaying_) {
        isPlaying_ = true;
        decodeTimer_->start();
        qDebug() << "Playback started";
    }
}

void MediaPlayer::pause() {
    if (isPlaying_) {
        isPlaying_ = false;
        decodeTimer_->stop();
        qDebug() << "Playback paused";
    }
}

bool MediaPlayer::isPlaying() const {
    return isPlaying_;
}

void MediaPlayer::decodingLoop() {
    if (!decoder_ || !converter_ || !decodeTimer_) {
        qDebug() << "decodingLoop: null pointers detected, stopping timer";
        if (decodeTimer_) {
            decodeTimer_->stop();
        }
        return;
    }

    if (!decoder_->isOpened()) {
        qDebug() << "decodingLoop: decoder not opened, stopping timer";
        decodeTimer_->stop();
        return;
    }

    if (!isPlaying_) {
        qDebug() << "decodingLoop: not playing, stopping timer";
        decodeTimer_->stop();
        return;
    }

    if (decoder_->readNextFrame()) {
        AVFrame *rawFrame = decoder_->getCurrentFrame();

        if (!rawFrame || !rawFrame->data[0]) {
            qDebug() << "decodingLoop: invalid frame received";
            return;
        }

        QImage frame = converter_->convertToQImage(rawFrame);

        if (frame.isNull()) {
            qDebug() << "decodingLoop: frame conversion failed";
            return;
        }

        emit frameReady(frame);

        int currentPosMs = decoder_->getCurrentPosition();

        if (!webCam_ && currentPosMs - lastPositionUpdateMs_ >= POSITION_UPDATE_INTERVAL_MS) {
            emit positionChanged(currentPosMs, durationMs_);
            lastPositionUpdateMs_ = currentPosMs;
        }
    } else {
        pause();
        emit playbackFinished();
    }
}

void MediaPlayer::seekTo(int positionMs) {
    if (!decoder_->isOpened() || durationMs_ == 0) return;

    if (positionMs < 0) positionMs = 0;
    if (positionMs > durationMs_) positionMs = durationMs_;

    AVRational timeBase = decoder_->getVideoTimeBase();
    int64_t timestamp = static_cast<int64_t>(positionMs / (av_q2d(timeBase) * 1000));

    if (decoder_->seekToPosition(timestamp)) {
        emit positionChanged(positionMs, durationMs_);

        qDebug() << "Seeked to" << positionMs << "ms";
    } else {
        qWarning() << "Failed to seek to" << positionMs << "ms";
    }
}

} // vsapp