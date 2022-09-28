#pragma once

#include <Windows.h>
#include "ffmpegcommondef.h"

// avcodec 模块导出函数
struct C_avcodec_ExportFuncs
{
public:
    bool getProcAddess(HMODULE hModule);

public:
    decltype(avcodec_find_decoder) *avcodec_find_decoderPtr;
    decltype(avcodec_find_decoder_by_name) *avcodec_find_decoder_by_namePtr;
    decltype(avcodec_alloc_context3) *avcodec_alloc_context3Ptr;
    decltype(avcodec_parameters_to_context) *avcodec_parameters_to_contextPtr;
    decltype(avcodec_parameters_free) *avcodec_parameters_freePtr;
    decltype(avcodec_open2) *avcodec_open2Ptr;
    decltype(avcodec_send_packet) *avcodec_send_packetPtr;
    decltype(avcodec_receive_frame) *avcodec_receive_framePtr;
    decltype(avcodec_flush_buffers) *avcodec_flush_buffersPtr;
    decltype(avcodec_free_context) *avcodec_free_contextPtr;
    decltype(av_packet_alloc) *av_packet_allocPtr;
    decltype(av_init_packet) *av_init_packetPtr;
    decltype(av_packet_unref) *av_packet_unrefPtr;
    decltype(av_packet_free) *av_packet_freePtr;
};
