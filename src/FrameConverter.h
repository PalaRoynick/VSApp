#pragma once
#include <QImage>

struct AVFrame;
struct SwsContext;

namespace vsapp {

class FrameConverter {
public:
    FrameConverter();
    ~FrameConverter();

    QImage convertToQImage(const AVFrame* frame);

private:
    SwsContext* swsContext_ = nullptr;
    uint8_t* rgbBuffer_ = nullptr;
    int bufferSize_ = 0;

    int srcWidth_ = 0;
    int srcHeight_ = 0;
    int srcFormat_ = -1;

    void cleanup();
    void initSwsContext(int width, int height, int format);
};

} // vsapp