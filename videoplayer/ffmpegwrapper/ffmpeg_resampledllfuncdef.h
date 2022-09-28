#pragma once

#include <Windows.h>
#include "ffmpegcommondef.h"

// swresample-0.dll 模块导出函数
struct C_swresample_ExportFuncs
{
public:
    bool getProcAddess(HMODULE hModule);

public:
    decltype(swr_alloc)* swr_allocPtr;
    decltype(swr_alloc_set_opts)* swr_alloc_set_optsPtr;
    decltype(swr_init)* swr_initPtr;
    decltype(swr_convert)* swr_convertPtr;
    decltype(swr_free)* swr_freePtr;
};
