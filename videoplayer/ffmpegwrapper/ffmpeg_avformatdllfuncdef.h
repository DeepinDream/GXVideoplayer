#pragma once

#include <Windows.h>
#include "ffmpegcommondef.h"

// avformat 模块导出函数
struct C_avformat_ExportFuncs
{
public:
    bool getProcAddess(HMODULE hModule);

public:
    decltype(av_register_all) *av_register_allPtr;
    decltype(avformat_alloc_context) *avformat_alloc_contextPtr;
    decltype(avformat_open_input) *avformat_open_inputPtr;
    decltype(avformat_find_stream_info) *avformat_find_stream_infoPtr;
    decltype(avformat_flush) * avformat_flushPtr;
    decltype(avformat_close_input) *avformat_close_inputPtr;
    decltype(av_read_frame) *av_read_framePtr;
    decltype(av_seek_frame) *av_seek_framePtr;
    decltype(av_dump_format) *av_dump_formatPtr;
    decltype(av_guess_frame_rate) *av_guess_frame_ratePtr;
    decltype(av_find_best_stream) *av_find_best_streamPtr;
};
