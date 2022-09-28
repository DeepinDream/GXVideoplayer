#include "ffmpeg_resampledllfuncdef.h"

bool C_swresample_ExportFuncs::getProcAddess(HMODULE hModule)
{
    GETPROCADDESS(hModule, swr_alloc);
    GETPROCADDESS(hModule, swr_alloc_set_opts);
    GETPROCADDESS(hModule, swr_init);
    GETPROCADDESS(hModule, swr_convert);
    GETPROCADDESS(hModule, swr_free);
    return true;
}
