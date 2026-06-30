extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <inttypes.h>
}

bool load_frame(const char* filename, int* width, int* height, unsigned char** data) {
    AVFormatContext* av_format_ctx = avformat_alloc_context();
    if (!av_format_ctx) {
        printf("Could not create AVFormatContext\n");
        return false;
    }

    if (avformat_open_input(&av_format_ctx, filename, NULL, NULL) != 0) {
        printf("Could not open video file\n");
        return false;
    }

    int video_stream_index = -1;
    AVCodecParameters* av_codec_params;
    AVCodec* av_codec;

    bool found = false;
    for (int i = 0; i < av_format_ctx->nb_streams; ++i) {
        av_codec_params = av_format_ctx->streams[i]->codecpar;
        av_codec = avcodec_find_decoder(av_codec_params->codec_id);

        if (!av_codec) {
            continue;
        }

        if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1) {
        printf("No video stream inside file is found\n");
        return false;
    }

    // set up a codec context for the decoder
    AVCodecContext* av_codec_ctx = avcodec_alloc_context3(av_codec);
    if (!av_codec_ctx) {
        printf("Could not create AVCodecContext\n");
        return false;
    }

    if (!avcodec_parameters_to_context(av_codec_ctx, av_codec_params)) {
        printf("Could not initialize AVCodecContext\n");
    }

    if (avcodec_open2(av_codec_ctx, av_codec, NULL) < 0) {
        printf("Could not open codec\n");
        return false;
    }

    avformat_close_input(&av_format_ctx);
    avformat_free_context(av_format_ctx);
    avcodec_free_context(&av_codec_ctx);

    return true;
}