#include "FrameConverter.h"

extern "C" {
#include <libavutil/frame.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include <QDebug>

namespace vsapp {

FrameConverter::FrameConverter() {}

FrameConverter::~FrameConverter() {
    cleanup();
}

void FrameConverter::cleanup() {
    sws_freeContext(swsContext_);
    swsContext_ = nullptr;
    if (rgbBuffer_) {
        av_freep(&rgbBuffer_); 
        rgbBuffer_ = nullptr;
    }
    srcWidth_ = 0;
    srcHeight_ = 0;
    srcFormat_ = -1;
}

void FrameConverter::initSwsContext(int width, int height, int format) {
    cleanup();

    bufferSize_ = av_image_get_buffer_size(AV_PIX_FMT_RGB32, width, height, 32);
    rgbBuffer_ = static_cast<uint8_t*>(av_malloc(bufferSize_));
    if (!rgbBuffer_) {
        qCritical() << "Failed to allocate RGB buffer";
        return;
    }

    swsContext_ = sws_getContext(
        width, height, static_cast<AVPixelFormat>(format),
        width, height, AV_PIX_FMT_RGB32,
        SWS_BILINEAR, 
        nullptr, nullptr, nullptr
    );

    if (!swsContext_) {
        qCritical() << "Failed to create SwsContext";
        av_freep(&rgbBuffer_);
        return;
    }

    srcWidth_ = width;
    srcHeight_ = height;
    srcFormat_ = format;
}

QImage FrameConverter::convertToQImage(const AVFrame* frame) {
    if (!frame) return {};

    int width  = frame->width;
    int height = frame->height;
    int format = frame->format;

    // if video changed its format or resolution, update on the fly
    if (!swsContext_ || width != srcWidth_ || height != srcHeight_ || format != srcFormat_) {
        initSwsContext(width, height, format);
    }

    if (!swsContext_ || !rgbBuffer_) {
        return {};
    }

    uint8_t* dstData[4] = { nullptr };
    int dstLinesize[4] = { 0 };

    av_image_fill_arrays(dstData, dstLinesize, rgbBuffer_, AV_PIX_FMT_RGB32, width, height, 32);

    const uint8_t *srcData[4] = { frame->data[0], frame->data[1], frame->data[2], frame->data[3] };
    const int srcLinesize[4]  = { frame->linesize[0], frame->linesize[1], frame->linesize[2], frame->linesize[3] };

    sws_scale(
        swsContext_,
        srcData, srcLinesize,
        0, height,
        dstData, dstLinesize
    );

    // QImage ctor does not own memory, so we call .copy() to copy pixels
    // into the inner memory for Qt handle lifetime itself
    QImage image(rgbBuffer_, width, height, dstLinesize[0], QImage::Format_RGB32);
    return image.copy();
}

} // vsapp