#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include "level2.h" // Make sure this file declares runLevel2
#include "easyMode.h"

int checkCollision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

void showRestartMessage(SDL_Renderer* renderer) {
    // Load the restart image
    SDL_Texture* restartTexture = IMG_LoadTexture(renderer, "restart.png");
    if (!restartTexture) {
        printf("Failed to load restart image! IMG_Error: %s\n", IMG_GetError());
        return; // Exit the function if image loading fails
    }

    // Set the position and size for the restart image
    SDL_Rect restartRect = { 0, 0, 640, 600 }; // Adjust the position and size as needed

    // Clear the screen (optional, if you want to clear the previous rendering)
    SDL_RenderClear(renderer);

    // Render the restart image
    SDL_RenderCopy(renderer, restartTexture, NULL, &restartRect);

    // Present the rendered image
    SDL_RenderPresent(renderer);

    // Clean up the texture
    SDL_DestroyTexture(restartTexture);
}

void playIntroVideo(SDL_Renderer* renderer) {
    // Initialize FFmpeg
    avformat_network_init();
    AVFormatContext* formatContext = NULL;

    if (avformat_open_input(&formatContext, "INTRO.mp4", NULL, NULL) != 0) {
        printf("Could not open video file\n");
        return;
    }

    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        printf("Could not find stream information\n");
        avformat_close_input(&formatContext);
        return;
    }

    int videoStreamIndex = -1;
    for (int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        printf("No video stream found\n");
        avformat_close_input(&formatContext);
        return;
    }

    AVCodecContext* codecContext = avcodec_alloc_context3(NULL);
    if (!codecContext) {
        printf("Failed to allocate codec context\n");
        avformat_close_input(&formatContext);
        return;
    }

    avcodec_parameters_to_context(codecContext, formatContext->streams[videoStreamIndex]->codecpar);

    AVCodec* codec = avcodec_find_decoder(codecContext->codec_id);
    if (codec == NULL) {
        printf("Codec not found\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        printf("Failed to open codec\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    AVPacket packet;
    AVFrame* frame = av_frame_alloc();
    AVFrame* rgbFrame = av_frame_alloc();
    if (!frame || !rgbFrame) {
        printf("Failed to allocate frames\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    if (!buffer) {
        printf("Failed to allocate buffer\n");
        av_frame_free(&frame);
        av_frame_free(&rgbFrame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGB24,
        codecContext->width, codecContext->height, 1);

    struct SwsContext* swsContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
        codecContext->width, codecContext->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL);
    if (!swsContext) {
        printf("Failed to create swsContext\n");
        av_freep(&buffer);
        av_frame_free(&frame);
        av_frame_free(&rgbFrame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == videoStreamIndex) {
            if (avcodec_send_packet(codecContext, &packet) == 0) {
                while (avcodec_receive_frame(codecContext, frame) == 0) {
                    sws_scale(swsContext, frame->data, frame->linesize, 0, codecContext->height,
                        rgbFrame->data, rgbFrame->linesize);

                    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                        SDL_TEXTUREACCESS_STREAMING,
                        codecContext->width, codecContext->height);
                    if (!texture) {
                        printf("Failed to create SDL texture: %s\n", SDL_GetError());
                        continue; // Proceed with next frame
                    }

                    SDL_UpdateTexture(texture, NULL, rgbFrame->data[0], rgbFrame->linesize[0]);

                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);

                    SDL_DestroyTexture(texture);
                    SDL_Delay(40);  // Adjust delay to match frame rate (frame rate is typically around 25-30 fps for videos)
                }
            }
        }
        av_packet_unref(&packet);
    }

    // Cleanup
    sws_freeContext(swsContext);
    av_freep(&buffer);
    av_frame_free(&frame);
    av_frame_free(&rgbFrame);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
}

void playcutscenehard(SDL_Renderer* renderer) {
    // Initialize FFmpeg
    avformat_network_init();
    AVFormatContext* formatContext = NULL;

    if (avformat_open_input(&formatContext, "cutscenehard.mp4", NULL, NULL) != 0) {
        printf("Could not open video file\n");
        return;
    }

    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        printf("Could not find stream information\n");
        avformat_close_input(&formatContext);
        return;
    }

    int videoStreamIndex = -1;
    for (int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        printf("No video stream found\n");
        avformat_close_input(&formatContext);
        return;
    }

    AVCodecContext* codecContext = avcodec_alloc_context3(NULL);
    if (!codecContext) {
        printf("Failed to allocate codec context\n");
        avformat_close_input(&formatContext);
        return;
    }

    avcodec_parameters_to_context(codecContext, formatContext->streams[videoStreamIndex]->codecpar);

    AVCodec* codec = avcodec_find_decoder(codecContext->codec_id);
    if (codec == NULL) {
        printf("Codec not found\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        printf("Failed to open codec\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    AVPacket packet;
    AVFrame* frame = av_frame_alloc();
    AVFrame* rgbFrame = av_frame_alloc();
    if (!frame || !rgbFrame) {
        printf("Failed to allocate frames\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    if (!buffer) {
        printf("Failed to allocate buffer\n");
        av_frame_free(&frame);
        av_frame_free(&rgbFrame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGB24,
        codecContext->width, codecContext->height, 1);

    struct SwsContext* swsContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
        codecContext->width, codecContext->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL);
    if (!swsContext) {
        printf("Failed to create swsContext\n");
        av_freep(&buffer);
        av_frame_free(&frame);
        av_frame_free(&rgbFrame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    SDL_Event event;
    int skipCutscene = 0;  // Flag to track if Enter was pressed to skip the cutscene

    while (av_read_frame(formatContext, &packet) >= 0 && !skipCutscene) {
        if (packet.stream_index == videoStreamIndex) {
            if (avcodec_send_packet(codecContext, &packet) == 0) {
                while (avcodec_receive_frame(codecContext, frame) == 0) {
                    sws_scale(swsContext, frame->data, frame->linesize, 0, codecContext->height,
                        rgbFrame->data, rgbFrame->linesize);

                    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                        SDL_TEXTUREACCESS_STREAMING,
                        codecContext->width, codecContext->height);
                    if (!texture) {
                        printf("Failed to create SDL texture: %s\n", SDL_GetError());
                        continue;  // Proceed with next frame
                    }

                    SDL_UpdateTexture(texture, NULL, rgbFrame->data[0], rgbFrame->linesize[0]);

                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);

                    SDL_DestroyTexture(texture);

                    SDL_Delay(40);  // Adjust delay to match frame rate (frame rate is typically around 25-30 fps for videos)

                    // Poll for events to check if the user pressed Enter to skip the cutscene
                    while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                            skipCutscene = 1;  // Exit if the window is closed
                        }
                        if (event.type == SDL_KEYDOWN) {
                            if (event.key.keysym.sym == SDLK_RETURN) {
                                skipCutscene = 1;  // Skip the cutscene if Enter is pressed
                            }
                        }
                    }
                }
            }
        }
        av_packet_unref(&packet);
    }

    // Cleanup
    sws_freeContext(swsContext);
    av_freep(&buffer);
    av_frame_free(&frame);
    av_frame_free(&rgbFrame);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
}


void playending(SDL_Renderer* renderer) {
    // Initialize FFmpeg
    avformat_network_init();
    AVFormatContext* formatContext = NULL;

    if (avformat_open_input(&formatContext, "ending.mp4", NULL, NULL) != 0) {
        printf("Could not open video file\n");
        return;
    }

    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        printf("Could not find stream information\n");
        avformat_close_input(&formatContext);
        return;
    }

    int videoStreamIndex = -1;
    for (int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        printf("No video stream found\n");
        avformat_close_input(&formatContext);
        return;
    }

    AVCodecContext* codecContext = avcodec_alloc_context3(NULL);
    if (!codecContext) {
        printf("Failed to allocate codec context\n");
        avformat_close_input(&formatContext);
        return;
    }

    avcodec_parameters_to_context(codecContext, formatContext->streams[videoStreamIndex]->codecpar);

    AVCodec* codec = avcodec_find_decoder(codecContext->codec_id);
    if (codec == NULL) {
        printf("Codec not found\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        printf("Failed to open codec\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    AVPacket packet;
    AVFrame* frame = av_frame_alloc();
    AVFrame* rgbFrame = av_frame_alloc();
    if (!frame || !rgbFrame) {
        printf("Failed to allocate frames\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    if (!buffer) {
        printf("Failed to allocate buffer\n");
        av_frame_free(&frame);
        av_frame_free(&rgbFrame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGB24,
        codecContext->width, codecContext->height, 1);

    struct SwsContext* swsContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
        codecContext->width, codecContext->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL);
    if (!swsContext) {
        printf("Failed to create swsContext\n");
        av_freep(&buffer);
        av_frame_free(&frame);
        av_frame_free(&rgbFrame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == videoStreamIndex) {
            if (avcodec_send_packet(codecContext, &packet) == 0) {
                while (avcodec_receive_frame(codecContext, frame) == 0) {
                    sws_scale(swsContext, frame->data, frame->linesize, 0, codecContext->height,
                        rgbFrame->data, rgbFrame->linesize);

                    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                        SDL_TEXTUREACCESS_STREAMING,
                        codecContext->width, codecContext->height);
                    if (!texture) {
                        printf("Failed to create SDL texture: %s\n", SDL_GetError());
                        continue; // Proceed with next frame
                    }

                    SDL_UpdateTexture(texture, NULL, rgbFrame->data[0], rgbFrame->linesize[0]);

                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);

                    SDL_DestroyTexture(texture);
                    SDL_Delay(40);  // Adjust delay to match frame rate (frame rate is typically around 25-30 fps for videos)
                }
            }
        }
        av_packet_unref(&packet);
    }

    // Cleanup
    sws_freeContext(swsContext);
    av_freep(&buffer);
    av_frame_free(&frame);
    av_frame_free(&rgbFrame);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
}
void playcutsceneez(SDL_Renderer* renderer) {
    // Initialize FFmpeg
    avformat_network_init();
    AVFormatContext* formatContext = NULL;

    if (avformat_open_input(&formatContext, "cutsceneez.mp4", NULL, NULL) != 0) {
        printf("Could not open video file\n");
        return;
    }

    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        printf("Could not find stream information\n");
        avformat_close_input(&formatContext);
        return;
    }

    int videoStreamIndex = -1;
    for (int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        printf("No video stream found\n");
        avformat_close_input(&formatContext);
        return;
    }

    AVCodecContext* codecContext = avcodec_alloc_context3(NULL);
    if (!codecContext) {
        printf("Failed to allocate codec context\n");
        avformat_close_input(&formatContext);
        return;
    }

    avcodec_parameters_to_context(codecContext, formatContext->streams[videoStreamIndex]->codecpar);

    AVCodec* codec = avcodec_find_decoder(codecContext->codec_id);
    if (codec == NULL) {
        printf("Codec not found\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        printf("Failed to open codec\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    AVPacket packet;
    AVFrame* frame = av_frame_alloc();
    AVFrame* rgbFrame = av_frame_alloc();
    if (!frame || !rgbFrame) {
        printf("Failed to allocate frames\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    if (!buffer) {
        printf("Failed to allocate buffer\n");
        av_frame_free(&frame);
        av_frame_free(&rgbFrame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGB24,
        codecContext->width, codecContext->height, 1);

    struct SwsContext* swsContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
        codecContext->width, codecContext->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL);
    if (!swsContext) {
        printf("Failed to create swsContext\n");
        av_freep(&buffer);
        av_frame_free(&frame);
        av_frame_free(&rgbFrame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    SDL_Event event;
    int skipCutscene = 0;  // Flag to track if Enter was pressed to skip the cutscene

    while (av_read_frame(formatContext, &packet) >= 0 && !skipCutscene) {
        if (packet.stream_index == videoStreamIndex) {
            if (avcodec_send_packet(codecContext, &packet) == 0) {
                while (avcodec_receive_frame(codecContext, frame) == 0) {
                    sws_scale(swsContext, frame->data, frame->linesize, 0, codecContext->height,
                        rgbFrame->data, rgbFrame->linesize);

                    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                        SDL_TEXTUREACCESS_STREAMING,
                        codecContext->width, codecContext->height);
                    if (!texture) {
                        printf("Failed to create SDL texture: %s\n", SDL_GetError());
                        continue;  // Proceed with next frame
                    }

                    SDL_UpdateTexture(texture, NULL, rgbFrame->data[0], rgbFrame->linesize[0]);

                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);

                    SDL_DestroyTexture(texture);

                    SDL_Delay(40);  // Adjust delay to match frame rate (frame rate is typically around 25-30 fps for videos)

                    // Poll for events to check if the user pressed Enter to skip the cutscene
                    while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                            skipCutscene = 1;  // Exit if the window is closed
                        }
                        if (event.type == SDL_KEYDOWN) {
                            if (event.key.keysym.sym == SDLK_RETURN) {
                                skipCutscene = 1;  // Skip the cutscene if Enter is pressed
                            }
                        }
                    }
                }
            }
        }
        av_packet_unref(&packet);
    }

    // Cleanup
    sws_freeContext(swsContext);
    av_freep(&buffer);
    av_frame_free(&frame);
    av_frame_free(&rgbFrame);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
}


void playbullethard(SDL_Renderer* renderer) {
    // Initialize FFmpeg
    avformat_network_init();
    AVFormatContext* formatContext = NULL;

    if (avformat_open_input(&formatContext, "bullethard.mp4", NULL, NULL) != 0) {
        printf("Could not open video file\n");
        return;
    }

    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        printf("Could not find stream information\n");
        avformat_close_input(&formatContext);
        return;
    }

    int videoStreamIndex = -1;
    for (int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        printf("No video stream found\n");
        avformat_close_input(&formatContext);
        return;
    }

    AVCodecContext* codecContext = avcodec_alloc_context3(NULL);
    if (!codecContext) {
        printf("Failed to allocate codec context\n");
        avformat_close_input(&formatContext);
        return;
    }

    avcodec_parameters_to_context(codecContext, formatContext->streams[videoStreamIndex]->codecpar);

    AVCodec* codec = avcodec_find_decoder(codecContext->codec_id);
    if (codec == NULL) {
        printf("Codec not found\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        printf("Failed to open codec\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    AVPacket packet;
    AVFrame* frame = av_frame_alloc();
    AVFrame* rgbFrame = av_frame_alloc();
    if (!frame || !rgbFrame) {
        printf("Failed to allocate frames\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    if (!buffer) {
        printf("Failed to allocate buffer\n");
        av_frame_free(&frame);
        av_frame_free(&rgbFrame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGB24,
        codecContext->width, codecContext->height, 1);

    struct SwsContext* swsContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
        codecContext->width, codecContext->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL);
    if (!swsContext) {
        printf("Failed to create swsContext\n");
        av_freep(&buffer);
        av_frame_free(&frame);
        av_frame_free(&rgbFrame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == videoStreamIndex) {
            if (avcodec_send_packet(codecContext, &packet) == 0) {
                while (avcodec_receive_frame(codecContext, frame) == 0) {
                    sws_scale(swsContext, frame->data, frame->linesize, 0, codecContext->height,
                        rgbFrame->data, rgbFrame->linesize);

                    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                        SDL_TEXTUREACCESS_STREAMING,
                        codecContext->width, codecContext->height);
                    if (!texture) {
                        printf("Failed to create SDL texture: %s\n", SDL_GetError());
                        continue; // Proceed with next frame
                    }

                    SDL_UpdateTexture(texture, NULL, rgbFrame->data[0], rgbFrame->linesize[0]);

                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);

                    SDL_DestroyTexture(texture);
                    SDL_Delay(40);  // Adjust delay to match frame rate (frame rate is typically around 25-30 fps for videos)
                }
            }
        }
        av_packet_unref(&packet);
    }

    // Cleanup
    sws_freeContext(swsContext);
    av_freep(&buffer);
    av_frame_free(&frame);
    av_frame_free(&rgbFrame);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
}

void playbulletez(SDL_Renderer* renderer) {
    // Initialize FFmpeg
    avformat_network_init();
    AVFormatContext* formatContext = NULL;

    if (avformat_open_input(&formatContext, "bulletez.mp4", NULL, NULL) != 0) {
        printf("Could not open video file\n");
        return;
    }

    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        printf("Could not find stream information\n");
        avformat_close_input(&formatContext);
        return;
    }

    int videoStreamIndex = -1;
    for (int i = 0; i < formatContext->nb_streams; i++) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        printf("No video stream found\n");
        avformat_close_input(&formatContext);
        return;
    }

    AVCodecContext* codecContext = avcodec_alloc_context3(NULL);
    if (!codecContext) {
        printf("Failed to allocate codec context\n");
        avformat_close_input(&formatContext);
        return;
    }

    avcodec_parameters_to_context(codecContext, formatContext->streams[videoStreamIndex]->codecpar);

    AVCodec* codec = avcodec_find_decoder(codecContext->codec_id);
    if (codec == NULL) {
        printf("Codec not found\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    if (avcodec_open2(codecContext, codec, NULL) < 0) {
        printf("Failed to open codec\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    AVPacket packet;
    AVFrame* frame = av_frame_alloc();
    AVFrame* rgbFrame = av_frame_alloc();
    if (!frame || !rgbFrame) {
        printf("Failed to allocate frames\n");
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, codecContext->width, codecContext->height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
    if (!buffer) {
        printf("Failed to allocate buffer\n");
        av_frame_free(&frame);
        av_frame_free(&rgbFrame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGB24,
        codecContext->width, codecContext->height, 1);

    struct SwsContext* swsContext = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt,
        codecContext->width, codecContext->height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, NULL, NULL, NULL);
    if (!swsContext) {
        printf("Failed to create swsContext\n");
        av_freep(&buffer);
        av_frame_free(&frame);
        av_frame_free(&rgbFrame);
        avcodec_free_context(&codecContext);
        avformat_close_input(&formatContext);
        return;
    }

    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == videoStreamIndex) {
            if (avcodec_send_packet(codecContext, &packet) == 0) {
                while (avcodec_receive_frame(codecContext, frame) == 0) {
                    sws_scale(swsContext, frame->data, frame->linesize, 0, codecContext->height,
                        rgbFrame->data, rgbFrame->linesize);

                    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                        SDL_TEXTUREACCESS_STREAMING,
                        codecContext->width, codecContext->height);
                    if (!texture) {
                        printf("Failed to create SDL texture: %s\n", SDL_GetError());
                        continue; // Proceed with next frame
                    }

                    SDL_UpdateTexture(texture, NULL, rgbFrame->data[0], rgbFrame->linesize[0]);

                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, texture, NULL, NULL);
                    SDL_RenderPresent(renderer);

                    SDL_DestroyTexture(texture);
                    SDL_Delay(40);  // Adjust delay to match frame rate (frame rate is typically around 25-30 fps for videos)
                }
            }
        }
        av_packet_unref(&packet);
    }

    // Cleanup
    sws_freeContext(swsContext);
    av_freep(&buffer);
    av_frame_free(&frame);
    av_frame_free(&rgbFrame);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
}


void showMainMenu(SDL_Renderer* renderer, int* play) {
    SDL_Event event;
    int menuRunning = 1;

    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "mainmenubg2.png");
    SDL_Texture* playTexture = IMG_LoadTexture(renderer, "play.png");
    SDL_Texture* quitTexture = IMG_LoadTexture(renderer, "quit.png");

    if (!bgTexture || !playTexture || !quitTexture) {
        printf("Failed to load textures! IMG_Error: %s\n", IMG_GetError());
        *play = 0;
        if (bgTexture) SDL_DestroyTexture(bgTexture);
        if (playTexture) SDL_DestroyTexture(playTexture);
        if (quitTexture) SDL_DestroyTexture(quitTexture);
        return;
    }

    // Original button sizes
    SDL_Rect playButton = { 220, 180, 250, 100 };
    SDL_Rect quitButton = { 220, 320, 250, 100 };
    SDL_Rect bgRect = { 0, 0, 640, 600 };

    // Enlarged versions of buttons for hover effect
    SDL_Rect enlargedPlayButton = { playButton.x - 10, playButton.y - 5, playButton.w + 20, playButton.h + 10 };
    SDL_Rect enlargedQuitButton = { quitButton.x - 10, quitButton.y - 5, quitButton.w + 20, quitButton.h + 10 };

    while (menuRunning) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);

        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if mouse is hovering over the play button and render the enlarged version if it is
        if (x >= playButton.x && x <= playButton.x + playButton.w &&
            y >= playButton.y && y <= playButton.y + playButton.h) {
            SDL_RenderCopy(renderer, playTexture, NULL, &enlargedPlayButton); // Render enlarged play button
        }
        else {
            SDL_RenderCopy(renderer, playTexture, NULL, &playButton); // Render normal play button
        }

        // Check if mouse is hovering over the quit button and render the enlarged version if it is
        if (x >= quitButton.x && x <= quitButton.x + quitButton.w &&
            y >= quitButton.y && y <= quitButton.y + quitButton.h) {
            SDL_RenderCopy(renderer, quitTexture, NULL, &enlargedQuitButton); // Render enlarged quit button
        }
        else {
            SDL_RenderCopy(renderer, quitTexture, NULL, &quitButton); // Render normal quit button
        }

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                *play = 0;
                menuRunning = 0;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                SDL_GetMouseState(&x, &y);
                if (x >= playButton.x && x <= playButton.x + playButton.w &&
                    y >= playButton.y && y <= playButton.y + playButton.h) {
                    showModeSelectionMenu(renderer, play);  // Show mode selection instead of starting game
                    menuRunning = 0;
                }
                else if (x >= quitButton.x && x <= quitButton.x + quitButton.w &&
                    y >= quitButton.y && y <= quitButton.y + quitButton.h) {
                    *play = 0;
                    menuRunning = 0;
                }
            }
        }
    }

    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(playTexture);
    SDL_DestroyTexture(quitTexture);
}


int showModeSelectionMenu(SDL_Renderer* renderer, int* startGame) {
    SDL_Event event;
    int modeMenuRunning = 1;

    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "mainmenubg2.png");
    SDL_Texture* easyModeTexture = IMG_LoadTexture(renderer, "easy.png");
    SDL_Texture* startGameTexture = IMG_LoadTexture(renderer, "hard.png");

    if (!bgTexture || !easyModeTexture || !startGameTexture) {
        printf("Failed to load textures! IMG_Error: %s\n", IMG_GetError());
        *startGame = 0;
        if (bgTexture) SDL_DestroyTexture(bgTexture);
        if (easyModeTexture) SDL_DestroyTexture(easyModeTexture);
        if (startGameTexture) SDL_DestroyTexture(startGameTexture);
        return 0;
    }

    // Original button sizes
    SDL_Rect easyModeButton = { 220, 180, 250, 100 };
    SDL_Rect startGameButton = { 220, 320, 250, 100 };
    SDL_Rect bgRect = { 0, 0, 640, 600 };

    // Enlarged versions of buttons for hover effect
    SDL_Rect enlargedEasyModeButton = { easyModeButton.x - 10, easyModeButton.y - 5, easyModeButton.w + 20, easyModeButton.h + 10 };
    SDL_Rect enlargedStartGameButton = { startGameButton.x - 10, startGameButton.y - 5, startGameButton.w + 20, startGameButton.h + 10 };

    while (modeMenuRunning) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);

        int x, y;
        SDL_GetMouseState(&x, &y);

        // Check if mouse is hovering over the easy mode button and render the enlarged version if it is
        if (x >= easyModeButton.x && x <= easyModeButton.x + easyModeButton.w &&
            y >= easyModeButton.y && y <= easyModeButton.y + easyModeButton.h) {
            SDL_RenderCopy(renderer, easyModeTexture, NULL, &enlargedEasyModeButton); // Render enlarged easy mode button
        }
        else {
            SDL_RenderCopy(renderer, easyModeTexture, NULL, &easyModeButton); // Render normal easy mode button
        }

        // Check if mouse is hovering over the start game button and render the enlarged version if it is
        if (x >= startGameButton.x && x <= startGameButton.x + startGameButton.w &&
            y >= startGameButton.y && y <= startGameButton.y + startGameButton.h) {
            SDL_RenderCopy(renderer, startGameTexture, NULL, &enlargedStartGameButton); // Render enlarged start game button
        }
        else {
            SDL_RenderCopy(renderer, startGameTexture, NULL, &startGameButton); // Render normal start game button
        }

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                *startGame = 0;
                modeMenuRunning = 0;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                SDL_GetMouseState(&x, &y);
                if (x >= easyModeButton.x && x <= easyModeButton.x + easyModeButton.w &&
                    y >= easyModeButton.y && y <= easyModeButton.y + easyModeButton.h) {
                    easyMode(renderer); // Call easy mode function
                    modeMenuRunning = 0;
                }
                else if (x >= startGameButton.x && x <= startGameButton.x + startGameButton.w &&
                    y >= startGameButton.y && y <= startGameButton.y + startGameButton.h) {
                    *startGame = 1;
                    modeMenuRunning = 0;
                }
            }
        }
    }

    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(easyModeTexture);
    SDL_DestroyTexture(startGameTexture);
    return 0;
}





int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("EON", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }



    // Load the background image
    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "bg.bmp");
    if (!bgTexture) {
        printf("Failed to load texture! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }


    // Load the static spike texture
    SDL_Texture* staticspikeTexture = IMG_LoadTexture(renderer, "spike.png");
    if (!staticspikeTexture) {
        printf("Failed to load spike image! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }

    // Load character image (single static image)
    SDL_Texture* characterTexture = IMG_LoadTexture(renderer, "character.png");
    if (!characterTexture) {
        printf("Failed to load character image! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }

    // Load the spike texture
    SDL_Texture* spikeTexture = IMG_LoadTexture(renderer, "movingspike.png");
    if (!spikeTexture) {
        printf("Failed to load spike image! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }
    // Play intro video before menu
    playIntroVideo(renderer);


    int play = 0;
    showMainMenu(renderer, &play);  // Display the main menu
    if (!play) {
        // Exit if "Quit" was selected
        SDL_DestroyTexture(bgTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 0;
    }
    playcutscenehard(renderer);

    //int level = 1;
    float squareSize = 50.0f, squareX = 160.0f, squareY = 100.0f, speed = 0.1f;
    float square2Size = 50.0f, square2X = 460.0f, square2Y = 100.0f, speed2 = 0.1f;

    // Initialize red boxes
    //2nd moving
    float redBoxSize = 30.0f, redBoxX = 0.0f, redBoxY = 430.0f, redBoxSpeed = 0.1f;
    int redBoxDirection = -1;
    //first moving
    float redBoxSize2 = 30.0f, redBoxX2 = 200.0f, redBoxY2 = 200.0f, redBoxSpeed2 = 0.1f;
    int redBoxDirection2 = 1;
    //last not moving
    float redBoxSizer = 30.0f, redBoxXr = 510.0f, redBoxYr = 420.0f, redBoxSpeedr = 0.1f;
    int redBoxDirectionr = -1;
    //first non moving
    float redBoxSize2r = 30.0f, redBoxX2r = 390.0f, redBoxY2r = 280.0f, redBoxSpeed2r = 0.1f;
    int redBoxDirection2r = 1;

    int wallx = 320, wally = 0, wallwidth = 20, wallheight = 600;

    int running = 1, gameOver = 0;
    SDL_Event event;

    // Define finish line rectangles
    SDL_Rect finishLineR = { 340, 482, 300, 119 };
    SDL_Rect finishLineL = { 0, 482, 320, 119 };

    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (gameOver && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
                // Reset positions and resume the game when 'R' is pressed after game over
                squareX = 160.0f;
                squareY = 100.0f;
                square2X = 460.0f;
                square2Y = 100.0f;
                redBoxX = 320.0f;
                redBoxX2 = 200.0f;
                redBoxXr = 510.0f;
                redBoxX2r = 390.0f;
                gameOver = 0;
            }
        }

        if (!gameOver) {
            // Handle keyboard input for square 1 (WASD keys)
            const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
            SDL_Rect square = { (int)squareX, (int)squareY, (int)squareSize, (int)squareSize };
            SDL_Rect square2 = { (int)square2X, (int)square2Y, (int)square2Size, (int)square2Size };
            SDL_Rect wall = { wallx, wally, wallwidth, wallheight };

            // Move the first square (WASD keys)
            if (currentKeyStates[SDL_SCANCODE_W] && squareY > 0) {
                squareY -= speed;  // Move up
            }
            if (currentKeyStates[SDL_SCANCODE_S] && squareY < 600) {
                squareY += speed;  // Move down
            }
            if (currentKeyStates[SDL_SCANCODE_A] && squareX > 0) {
                squareX -= speed;  // Move left
            }
            if (currentKeyStates[SDL_SCANCODE_D] && squareX < 260) {
                squareX += speed;  // Move right
            }

            // Move the second square (Arrow keys)
            if (currentKeyStates[SDL_SCANCODE_W] && square2Y > 0) {
                square2Y -= speed;  // Move up
            }
            if (currentKeyStates[SDL_SCANCODE_S] && square2Y < 600) {
                square2Y += speed;  // Move down
            }
            if (currentKeyStates[SDL_SCANCODE_A] && square2X > 330) {
                square2X -= speed;  // Move left
            }
            if (currentKeyStates[SDL_SCANCODE_D] && square2X < 640 - squareSize) {
                square2X += speed;  // Move right
            }

            // Move the red boxes horizontally for left wall
            //last box
            redBoxX += redBoxDirection * redBoxSpeed;
            if (redBoxX <= 0) redBoxDirection = 1;
            else if (redBoxX >= 320 - redBoxSize) redBoxDirection = -1;

            //first box
            redBoxX2 += redBoxDirection2 * redBoxSpeed2;
            if (redBoxX2 <= 0) redBoxDirection2 = 1;
            else if (redBoxX2 >= 320 - redBoxSize2) redBoxDirection2 = -1;

            // Check for collisions
            SDL_Rect redBox = { (int)redBoxX, (int)redBoxY, (int)redBoxSize, (int)redBoxSize };
            SDL_Rect redBox2 = { (int)redBoxX2, (int)redBoxY2, (int)redBoxSize2, (int)redBoxSize2 };
            SDL_Rect redBoxr = { (int)redBoxXr, (int)redBoxYr, (int)redBoxSizer, (int)redBoxSizer };
            SDL_Rect redBox2r = { (int)redBoxX2r, (int)redBoxY2r, (int)redBoxSize2r, (int)redBoxSize2r };

            if (checkCollision(square, redBox) || checkCollision(square, redBox2) || checkCollision(square2, redBoxr) || checkCollision(square2, redBox2r)) {
                gameOver = 1;
            }


            if (checkCollision(square2, finishLineR)) {
                int level2status = runLevel2(renderer);
                if (level2status == 1) {
                    gameOver = 1;
                    continue;
                }
                break;
            }

            // Clear the window and render the background
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, bgTexture, NULL, NULL); // Render the background texture




                        // Draw the character (use the single static character image)
            SDL_RenderCopy(renderer, characterTexture, NULL, &square);
            SDL_RenderCopy(renderer, characterTexture, NULL, &square2);


            SDL_RenderCopy(renderer, spikeTexture, NULL, &redBox);
            SDL_RenderCopy(renderer, spikeTexture, NULL, &redBox2);
            SDL_RenderCopy(renderer, staticspikeTexture, NULL, &redBoxr);
            SDL_RenderCopy(renderer, staticspikeTexture, NULL, &redBox2r);


            SDL_RenderPresent(renderer);
        }
        else {
            // Show restart message
            showRestartMessage(renderer);
        }
    }

    // Cleanup
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(spikeTexture);
    SDL_DestroyTexture(staticspikeTexture);
    SDL_DestroyTexture(characterTexture);
    IMG_Quit(); // Quit SDL_image
    SDL_Quit();
    return 0;
}
