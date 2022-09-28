#pragma once

#include <Windows.h>
#include "ffmpegcommondef.h"

// swscale 模块导出函数
struct C_swscale_ExportFuncs
{
public:
    bool getProcAddess(HMODULE hModule);

public:
    decltype(sws_getContext) *sws_getContextPtr;
    decltype(sws_scale) *sws_scalePtr;
    decltype(sws_freeContext) *sws_freeContextPtr;
};
