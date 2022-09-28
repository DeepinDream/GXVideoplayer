#ifndef AUDIORESAMPLE_H
#define AUDIORESAMPLE_H

#include "ffmpegwrapper/ffmpegdllfuncdef.h"

struct AVCodecParameters;
struct SwrContext;
struct AVFrame;

class AudioResample
{
public:
    AudioResample();
    ~AudioResample();
    bool open(AVCodecParameters* pPara);
    void close();
    int resample(AVFrame* pInputData,unsigned char* pOutputData);

private:
    SwrContext* m_pSwrctx;
    int m_nOutFormat; //AVSampleFormat AV_SAMPLE_FMT_S16 默认为1

    C_avutil_ExportFuncs m_avutilFuncs;
    C_swresample_ExportFuncs m_swresampleFuncs;
};

#endif // AUDIORESAMPLE_H
