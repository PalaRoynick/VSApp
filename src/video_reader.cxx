extern "C" {
#include <libavdevice/avdevice.h>
}

#include "video_reader.hpp"

bool video_reader_open(VideoReaderState* state, const char* filename) {

    avdevice_register_all();

    auto& width = state->width;
    auto& height = state->height;
    auto& time_base = state->time_base;
    auto& av_format_ctx = state->av_format_ctx;
    auto& av_codec_ctx = state->av_codec_ctx;
    auto& video_stream_index = state->video_stream_index;
    auto& av_frame = state->av_frame;
    auto& av_packet = state->av_packet;

    av_format_ctx = avformat_alloc_context();
    if (!av_format_ctx) {
        printf("Could not create AVFormatContext\n");
        return false;
    }

    const AVInputFormat* av_input_format = NULL;
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

    // Avoid possible "corrupted data": these options work on my Windows + WSL2 machine
    AVDictionary *options = NULL;
    av_dict_set(&options, "input_format", "mjpeg", 0);
    av_dict_set(&options, "video_size", "640x480", 0);
    av_dict_set(&options, "framerate", "30", 0);
    av_dict_set(&options, "pix_fmt", "yuv420p", 0);
    
    if (avformat_open_input(&av_format_ctx, "/dev/video0", av_input_format, &options) != 0) {
        printf("Could not open video file\n");
        // av_dict_free(&options);
        return false;
    }

    // av_dict_free(&options);

    if (avformat_find_stream_info(av_format_ctx, NULL) < 0) {
        printf("Could not find stream information\n");
        return false;
    }

    video_stream_index = -1;
    AVCodecParameters* av_codec_params = NULL;
    const AVCodec* av_codec = NULL;

    for (int i = 0; i < av_format_ctx->nb_streams; ++i) {
        av_codec_params = av_format_ctx->streams[i]->codecpar;
        av_codec = avcodec_find_decoder(av_codec_params->codec_id);

        if (!av_codec) {
            continue;
        }

        if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            width = av_codec_params->width;
            height = av_codec_params->height;
            time_base = av_format_ctx->streams[i]->time_base;
            break;
        }
    }

    if (video_stream_index == -1) {
        printf("No video stream inside file is found\n");
        return false;
    }

    // set up a codec context for the decoder
    av_codec_ctx = avcodec_alloc_context3(av_codec);
    if (!av_codec_ctx) {
        printf("Could not create AVCodecContext\n");
        return false;
    }

    if (avcodec_parameters_to_context(av_codec_ctx, av_codec_params) < 0) {
        printf("Could not initialize AVCodecContext\n");
    }

    if (avcodec_open2(av_codec_ctx, av_codec, NULL) < 0) {
        printf("Could not open codec\n");
        return false;
    }

    av_frame = av_frame_alloc();
    if (!av_frame) {
        printf("Could not allocate AVFrame\n");
        return false;
    }

    av_packet = av_packet_alloc();
    if (!av_packet) {
        printf("Could not allocate AVPacket\n");
        return false;
    }

    return true;
}

bool video_reader_read_frame(VideoReaderState* state, uint8_t* frame_buffer) {
    auto& width = state->width;
    auto& height = state->height;
    auto& av_format_ctx = state->av_format_ctx;
    auto& av_codec_ctx = state->av_codec_ctx;
    auto& video_stream_index = state->video_stream_index;
    auto& av_frame = state->av_frame;
    auto& av_packet = state->av_packet;
    auto& sws_scaler_ctx = state->sws_scaler_ctx;

    // we are looking for a packet for the video stream only
    int response;

    while (av_read_frame(av_format_ctx, av_packet) == 0) {
        if (av_packet->stream_index != video_stream_index) {
            av_packet_unref(av_packet);
            continue;
        }

        // I had an issue here because v4l2.c explicitly sets packet size to 0 when 
        // it mets corrupted data. Just skip such a packet.
        if (av_packet->size == 0) {
            av_packet_unref(av_packet);
            continue;
        }

        response = avcodec_send_packet(av_codec_ctx, av_packet);
        if (response < 0) {
            char errbuf[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(errbuf, sizeof(errbuf), response);
            printf("Failed to decode packet : %s\n", errbuf);
            return false;
        }

        response = avcodec_receive_frame(av_codec_ctx, av_frame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
            av_packet_unref(av_packet);
            continue;
        } else if (response < 0) {
            char errbuf[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(errbuf, sizeof(errbuf), response);
            printf("Failed to decode frame : %s\n", errbuf);
            return false;
        }

        av_packet_unref(av_packet);
        break;
    }

    if (!sws_scaler_ctx) {
        sws_scaler_ctx = sws_getContext(
            width,
            height,
            av_codec_ctx->pix_fmt,
            width,
            height,
            AV_PIX_FMT_RGB0,
            SWS_BILINEAR,
            NULL,
            NULL,
            NULL
        );
    }

    if (!sws_scaler_ctx) {
        printf("Could not initialize sw scaler\n");
        return false;
    }

    uint8_t* dest[4] = { frame_buffer, NULL, NULL, NULL };
    int dest_linesize[4] = { width * 4, 0, 0, 0 };
    sws_scale(sws_scaler_ctx, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_linesize);

    return true;
}

void video_reader_close(VideoReaderState* state) {
    sws_freeContext(state->sws_scaler_ctx);
    avformat_close_input(&state->av_format_ctx);
    avformat_free_context(state->av_format_ctx);
    av_frame_free(&state->av_frame);
    av_packet_free(&state->av_packet);
    avcodec_free_context(&state->av_codec_ctx);
}