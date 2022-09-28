#include "ffmpeg_avformatdllfuncdef.h"

bool C_avformat_ExportFuncs::getProcAddess(HMODULE hModule)
{
    GETPROCADDESS(hModule, av_register_all);
    GETPROCADDESS(hModule, avformat_alloc_context);
    GETPROCADDESS(hModule, avformat_open_input);
    GETPROCADDESS(hModule, avformat_find_stream_info);
    GETPROCADDESS(hModule, avformat_flush);
    GETPROCADDESS(hModule, avformat_close_input);
    GETPROCADDESS(hModule, av_read_frame);
    GETPROCADDESS(hModule, av_seek_frame);
    GETPROCADDESS(hModule, av_dump_format);
    GETPROCADDESS(hModule, av_guess_frame_rate);
    GETPROCADDESS(hModule, av_find_best_stream);
    return true;
}
