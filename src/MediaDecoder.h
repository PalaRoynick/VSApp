#pragma once
#include <string>

extern "C" {
#include <libavutil/rational.h>
}

struct AVFormatContext;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;

namespace vsapp {

class MediaDecoder {
public:
    MediaDecoder();
    ~MediaDecoder();

    bool openFile(const std::string &filePath, bool webCam = false);
    bool readNextFrame(); 
    
    AVFrame* getCurrentFrame() const;
    int getWidth() const;
    int getHeight() const;
    double getFps() const;
    bool isOpened() const;

    int64_t getCurrentPosition() const;
    int64_t getDuration() const;
    AVRational getVideoTimeBase() const;
    bool seekToPosition(int64_t timestamp);

private:
    AVFormatContext *formatCtx_ = nullptr;
    AVCodecContext  *codecCtx_ = nullptr;
    AVFrame         *frame_ = nullptr;
    AVPacket        *packet_ = nullptr;
    
    int videoStreamIndex_ = -1;
    bool isOpened_ = false;
    
    void cleanup();
};

} // vsapp
