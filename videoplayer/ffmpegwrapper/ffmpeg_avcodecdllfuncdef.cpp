#include "ffmpeg_avcodecdllfuncdef.h"

bool C_avcodec_ExportFuncs::getProcAddess(HMODULE hModule)
{
    GETPROCADDESS(hModule, avcodec_find_decoder);
    GETPROCADDESS(hModule, avcodec_find_decoder_by_name);
    GETPROCADDESS(hModule, avcodec_alloc_context3);
    GETPROCADDESS(hModule, avcodec_parameters_to_context);
    GETPROCADDESS(hModule, avcodec_parameters_free);
    GETPROCADDESS(hModule, avcodec_open2);
    GETPROCADDESS(hModule, avcodec_send_packet);
    GETPROCADDESS(hModule, avcodec_receive_frame);
    GETPROCADDESS(hModule, avcodec_flush_buffers);
    GETPROCADDESS(hModule, avcodec_free_context);
    GETPROCADDESS(hModule, av_packet_alloc);
    GETPROCADDESS(hModule, av_init_packet);
    GETPROCADDESS(hModule, av_packet_unref);
    GETPROCADDESS(hModule, av_packet_free);
    return true;
}
