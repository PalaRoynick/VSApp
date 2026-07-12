#include "MediaDecoder.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavdevice/avdevice.h>
}

#include <iostream>

namespace vsapp {

MediaDecoder::MediaDecoder() {
    avformat_network_init();
}

MediaDecoder::~MediaDecoder() {
    cleanup();
}

void MediaDecoder::cleanup() {
    av_frame_free(&frame_);
    av_packet_free(&packet_);
    avcodec_free_context(&codecCtx_);
    avformat_close_input(&formatCtx_);
    
    videoStreamIndex_ = -1;
    isOpened_ = false;
}

bool MediaDecoder::openFile(const std::string &filePath, bool webCam) {
    cleanup();

    const AVInputFormat* av_input_format = NULL;
    if  (webCam) {
        do {
            av_input_format = av_input_video_device_next(av_input_format);
            if (av_input_format) {
                printf("INPUT FORMAT: [%s] %s\n", av_input_format->name, av_input_format->long_name);
            }
        } while (av_input_format && strcmp(av_input_format->long_name, "Video4Linux2 device grab") != 0);

        if (!av_input_format) {
            printf("Could not find v4l2 input format to get webcam\n");
            return false;
        }
    }

    // Avoid possible "corrupted data": these options work on my Windows + WSL2 machine
    AVDictionary *options = NULL;
    if (webCam) {
        av_dict_set(&options, "input_format", "mjpeg", 0);
        av_dict_set(&options, "video_size", "640x480", 0);
        av_dict_set(&options, "framerate", "30", 0);
        av_dict_set(&options, "pix_fmt", "yuv420p", 0);
    }

    if (avformat_open_input(&formatCtx_, filePath.c_str(), av_input_format, &options) != 0) {
        printf("Could not open video file\n");
        av_dict_free(&options);
        return false;
    }

    av_dict_free(&options);

    if (avformat_find_stream_info(formatCtx_, nullptr) < 0) {
        std::cerr << "Error: Could not find stream information" << std::endl;
        cleanup();
        return false;
    }

    videoStreamIndex_ = av_find_best_stream(formatCtx_, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    if (videoStreamIndex_ < 0) {
        std::cerr << "Error: Could not find video stream in the input" << std::endl;
        cleanup();
        return false;
    }

    AVCodecParameters *codecParams = formatCtx_->streams[videoStreamIndex_]->codecpar;
    const AVCodec *codec = avcodec_find_decoder(codecParams->codec_id);
    if (!codec) {
        std::cerr << "Error: Failed to find decoder for codec ID " << codecParams->codec_id << std::endl;
        cleanup();
        return false;
    }

    codecCtx_ = avcodec_alloc_context3(codec);
    if (!codecCtx_ || avcodec_parameters_to_context(codecCtx_, codecParams) < 0) {
        std::cerr << "Error: Failed to allocate or setup codec context" << std::endl;
        cleanup();
        return false;
    }

    if (avcodec_open2(codecCtx_, codec, nullptr) < 0) {
        std::cerr << "Error: Failed to open codec" << std::endl;
        cleanup();
        return false;
    }

    frame_ = av_frame_alloc();
    packet_ = av_packet_alloc();
    if (!frame_ || !packet_) {
        std::cerr << "Error: Failed to allocate frame or packet" << std::endl;
        cleanup();
        return false;
    }

    isOpened_ = true;
    return true;
}

bool MediaDecoder::readNextFrame() {
    if (!isOpened_) {
        std::cerr << "readNextFrame: decoder not opened" << std::endl;
        return false;
    }

    int receiveRet = avcodec_receive_frame(codecCtx_, frame_);

    if (receiveRet == 0) {
        return true;
    } else if (receiveRet != AVERROR(EAGAIN)) {
        std::cerr << "readNextFrame: avcodec_receive_frame failed with error: " << receiveRet << std::endl;
        return false;
    }

    while (av_read_frame(formatCtx_, packet_) >= 0) {
        if (packet_->stream_index != videoStreamIndex_) {
            av_packet_unref(packet_);
            continue;
        }

        int sendRet = avcodec_send_packet(codecCtx_, packet_);
        av_packet_unref(packet_);

        if (sendRet < 0) {
            continue;
        }

        receiveRet = avcodec_receive_frame(codecCtx_, frame_);
        
        if (receiveRet == 0) {
            return true;
        } else if (receiveRet == AVERROR(EAGAIN)) {
            continue;
        } else {
            std::cerr << "readNextFrame: avcodec_receive_frame failed after send_packet" << std::endl;
            return false;
        }
    }

    avcodec_send_packet(codecCtx_, nullptr);

    if (avcodec_receive_frame(codecCtx_, frame_) == 0) {
        return true;
    }

    return false;
}

AVFrame* MediaDecoder::getCurrentFrame() const {
    return frame_;
}

int MediaDecoder::getWidth() const {
    return codecCtx_ ? codecCtx_->width : 0;
}

int MediaDecoder::getHeight() const {
    return codecCtx_ ? codecCtx_->height : 0;
}

double MediaDecoder::getFps() const {
    if (!formatCtx_ || videoStreamIndex_ < 0) return 0.0;

    AVRational avgFps = formatCtx_->streams[videoStreamIndex_]->avg_frame_rate;
    if (avgFps.den > 0 && avgFps.num > 0) {
        return av_q2d(avgFps);
    }

    AVRational baseFps = formatCtx_->streams[videoStreamIndex_]->r_frame_rate;
    if (baseFps.den > 0 && baseFps.num > 0) {
        return av_q2d(baseFps);
    }

    return 0.0;
}

bool MediaDecoder::isOpened() const {
    return isOpened_;
}

int64_t MediaDecoder::getCurrentPosition() const {
    if (!isOpened_ || !frame_) return 0;

    if (frame_->pts == AV_NOPTS_VALUE) {
        return 0;
    }

    AVRational timeBase = formatCtx_->streams[videoStreamIndex_]->time_base;

    double ptsInSeconds = frame_->pts * av_q2d(timeBase);
    return static_cast<int64_t>(ptsInSeconds * 1000);
}

int64_t MediaDecoder::getDuration() const {
    if (!isOpened_) return 0;
    return formatCtx_->duration / 1000;
}

AVRational MediaDecoder::getVideoTimeBase() const {
    if (!isOpened_ || videoStreamIndex_ < 0) {
        return {1, 1};
    }
    return formatCtx_->streams[videoStreamIndex_]->time_base;
}

bool MediaDecoder::seekToPosition(int64_t timestamp) {
    if (!isOpened_) return false;

    int ret = av_seek_frame(
        formatCtx_, 
        videoStreamIndex_, 
        timestamp, 
        AVSEEK_FLAG_BACKWARD
    );

    if (ret < 0) {
        std::cerr << "Error: Seek failed" << std::endl;
        return false;
    }

    avcodec_flush_buffers(codecCtx_);

    return true;
}

} // vsapp