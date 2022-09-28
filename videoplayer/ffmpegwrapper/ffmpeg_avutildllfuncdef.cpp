#include "ffmpeg_avutildllfuncdef.h"

bool C_avutil_ExportFuncs::getProcAddess(HMODULE hModule)
{
    GETPROCADDESS(hModule, av_frame_alloc);
    GETPROCADDESS(hModule, av_frame_free);
    GETPROCADDESS(hModule, av_malloc);
    GETPROCADDESS(hModule, av_free);
    GETPROCADDESS(hModule, av_image_get_buffer_size);
    GETPROCADDESS(hModule, av_image_fill_arrays);
    GETPROCADDESS(hModule, av_strerror);
    GETPROCADDESS(hModule, av_get_default_channel_layout);
    GETPROCADDESS(hModule, av_get_bytes_per_sample);
    GETPROCADDESS(hModule, av_buffer_unref);
    return true;
}
