#pragma once

#define __STDC_CONSTANT_MACROS
#define UINT64_C(val) val##ui64
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/channel_layout.h"
#include "libavutil/mem.h"
#include "libavutil/error.h"
#include "libavutil/frame.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
};

#define GETPROCADDESS(module, name)                                 \
{                                                                   \
    name ## Ptr = (decltype(name) *)GetProcAddress(module, #name);  \
    if (name ## Ptr == 0)                                           \
    {                                                               \
        return false;                                               \
    }                                                               \
}
