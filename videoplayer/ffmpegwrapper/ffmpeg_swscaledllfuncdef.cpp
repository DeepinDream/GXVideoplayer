#include "ffmpeg_swscaledllfuncdef.h"

bool C_swscale_ExportFuncs::getProcAddess(HMODULE hModule)
{
    GETPROCADDESS(hModule, sws_getContext);
    GETPROCADDESS(hModule, sws_scale);
    GETPROCADDESS(hModule, sws_freeContext);
    return true;
}
