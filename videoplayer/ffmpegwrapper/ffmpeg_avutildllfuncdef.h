#pragma once

#include <Windows.h>
#include "ffmpegcommondef.h"

// avutil 模块导出函数
struct C_avutil_ExportFuncs
{
public:
    bool getProcAddess(HMODULE hModule);

public:
    decltype(av_frame_alloc) *av_frame_allocPtr;
    decltype(av_frame_free) *av_frame_freePtr;
    decltype(av_malloc) *av_mallocPtr;
    decltype(av_free) *av_freePtr;
    decltype(av_image_get_buffer_size) *av_image_get_buffer_sizePtr;
    decltype(av_image_fill_arrays) *av_image_fill_arraysPtr;
    decltype(av_strerror) *av_strerrorPtr;
    decltype(av_get_default_channel_layout) *av_get_default_channel_layoutPtr;
    decltype(av_get_bytes_per_sample) *av_get_bytes_per_samplePtr;
    decltype(av_buffer_unref) *av_buffer_unrefPtr;
};
