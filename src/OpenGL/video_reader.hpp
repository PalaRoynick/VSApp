#pragma once

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}

struct VideoReaderState {
    // public things for other parts of the program
    int width = 0, height = 0;
    AVRational time_base;

    // private internal state
    AVFormatContext* av_format_ctx       = NULL;
    AVCodecContext*  av_codec_ctx        = NULL;
    int              video_stream_index  =    0;
    AVFrame*         av_frame            = NULL;
    AVPacket*        av_packet           = NULL;
    SwsContext*      sws_scaler_ctx      = NULL;
};

bool video_reader_open(VideoReaderState* state, const char* filename);

bool video_reader_read_frame(VideoReaderState* state, uint8_t* frame_buffer);

void video_reader_close(VideoReaderState* state);
