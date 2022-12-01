#include "audioresample.h"
#include "ffmpegwrapper/ffmpegwrapper.h"

#include <QDebug>

AudioResample::AudioResample()
    : m_pSwrctx(nullptr)
    , m_nOutFormat(1)
{
    m_avutilFuncs = FFMPegWrapper::instance().get_avutil_ExportFuncs();
    m_swresampleFuncs = FFMPegWrapper::instance().get_swrespamle_ExportFuncs();
}

AudioResample::~AudioResample()
{
    close();
}

bool AudioResample::open(AVCodecParameters *pPara)
{
    if(!pPara)
        return false;
    m_nOutFormat = pPara->format == AV_SAMPLE_FMT_FLTP ? AV_SAMPLE_FMT_S16 : m_nOutFormat;
    m_pSwrctx = m_swresampleFuncs.swr_alloc_set_optsPtr(m_pSwrctx,
                                                        pPara->channel_layout,                  // 输出格式
                                                        (AVSampleFormat)m_nOutFormat,                                       // 输出样本格式
                                                        pPara->sample_rate,                                                 // 输出采样率
                                                        pPara->channel_layout,    // 输入格式
                                                        (AVSampleFormat)pPara->format,
                                                        pPara->sample_rate,
                                                        0,0);
    int nRet = m_swresampleFuncs.swr_initPtr(m_pSwrctx);
    if(nRet!=0)
    {
        char buff[1024] = {0};
        m_avutilFuncs.av_strerrorPtr(nRet,buff,sizeof(buff)-1);
        qDebug()<<"swr_init failed! :"<<buff;
        close();
        return false;
    }
    return true;
}

void AudioResample::close()
{
    if(m_pSwrctx)
        m_swresampleFuncs.swr_freePtr(&m_pSwrctx);
}

int AudioResample::resample(AVFrame *pInputData, unsigned char *pOutputData)
{
    if(!pInputData)
        return 0;
    if(!pOutputData)
    {
        return 0;
    }
    uint8_t* data[2] = {0};
    data[0] = pOutputData;
    int nRet = m_swresampleFuncs.swr_convertPtr(m_pSwrctx,
                   data,pInputData->nb_samples,
                   (const uint8_t**)pInputData->data,pInputData->nb_samples);
    int nOutSize = nRet * pInputData->channels * m_avutilFuncs.av_get_bytes_per_samplePtr((AVSampleFormat)m_nOutFormat);
    if(nRet<=0)
        return nRet;

    return nOutSize;
}
