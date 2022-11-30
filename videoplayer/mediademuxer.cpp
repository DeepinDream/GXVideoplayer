#include "mediademuxer.h"
#include "myavpacket.h"
#include "ffmpegwrapper/ffmpegwrapper.h"

#include <QTextCodec>
#include <QDebug>

#define CHECK_FORMATCONTEXT(ret) if(!m_formatCtx){return ret;}

static double r2d(AVRational r)
{
    return r.den == 0 ? 0 :static_cast<double>(r.num)/static_cast<double>(r.den);
}

int getFrameIntervalms(int num, int den)
{
    int intervalms = 50; //默认20帧/s
    if (num > 0 && den > 0 && num > den)
    {
        intervalms = 1000 * den / num;
    }
    return intervalms;
}

MediaDemuxer::MediaDemuxer()
    : m_formatCtx(NULL)
    , m_packet(NULL)
    , m_videoStreamIdx(-1)
    , m_audioStreamIdx(-1)
    , m_width(0)
    , m_height(0)
    , m_sampleRate(0)
    , m_channels(0)
{
    m_avformatFuncs = FFMPegWrapper::instance().get_avformat_ExportFuncs();
    m_avutilFuncs = FFMPegWrapper::instance().get_avutil_ExportFuncs();

    m_packet = new MyAVPacket();
}

MediaDemuxer::~MediaDemuxer()
{
    close();
}

bool MediaDemuxer::open(const QString &strUrl)
{
    int errorCode = 0;
    char errorBuf[1024] = { 0 };
    m_formatCtx = m_avformatFuncs.avformat_alloc_contextPtr();

    QByteArray curPath = QTextCodec::codecForName("System")->fromUnicode(strUrl);
    errorCode = m_avformatFuncs.avformat_open_inputPtr(&m_formatCtx, curPath.constData(), NULL, NULL);
    if (errorCode != 0)
    {
        m_avutilFuncs.av_strerrorPtr(errorCode, errorBuf, sizeof(errorBuf));
        qDebug() << "avformat_open_input error, mbcs path: " << strUrl
                 << ", error code: " << errorCode << ", errorBuf: " << errorBuf;

        // ffmpeg文档没有明确说明avformat_open_input中的输入是否要求utf8编码，保险起见先尝试多字节，然后尝试utf8
        curPath = QTextCodec::codecForName("UTF-8")->fromUnicode(strUrl);
        errorCode = m_avformatFuncs.avformat_open_inputPtr(&m_formatCtx, curPath.constData(), NULL, NULL);
        if (errorCode != 0)
        {
            m_avutilFuncs.av_strerrorPtr(errorCode, errorBuf, sizeof(errorBuf));
            qDebug() << "avformat_open_input error, utf8 path: " << strUrl
                     << ", error code: " << errorCode << ", errorBuf: " << errorBuf;
            return false;
        }
    }
    errorCode = m_avformatFuncs.avformat_find_stream_infoPtr(m_formatCtx,nullptr);
    if(errorCode != 0){
        m_avutilFuncs.av_strerrorPtr(errorCode, errorBuf, sizeof(errorBuf));
        qDebug()<<"find stream failed, "<< errorCode<< ", errorBuf: " << errorBuf;
        return false;
    }

    // 总时长
    m_totalTimeMs = m_formatCtx->duration/(AV_TIME_BASE/1000);
    qDebug()<<"total time ms = "<<m_totalTimeMs;

    // dump 详细信息
    m_avformatFuncs.av_dump_formatPtr(m_formatCtx,0,curPath.constData(),0);

    updateVideoInfo(m_formatCtx);
    updateAudioInfo(m_formatCtx);
    return true;
}

void MediaDemuxer::close()
{
    if(m_formatCtx){
        m_avformatFuncs.avformat_close_inputPtr(&m_formatCtx);
        m_formatCtx = NULL;
    }
    if(m_packet){
        delete m_packet;
        m_packet = NULL;
    }
    m_videoStreamIdx = -1;
    m_audioStreamIdx = -1;
    m_width = 0;
    m_height = 0;
    m_sampleRate = 0;
    m_channels = 0;
}

bool MediaDemuxer::seek(double dPos)
{
    CHECK_FORMATCONTEXT(false);

    m_avformatFuncs.avformat_flushPtr(m_formatCtx);

    double sec = m_totalTimeMs * dPos / 1000;

    int nRet = -1;
    if(m_videoStreamIdx >= 0){
        int64_t pts = sec / r2d(m_formatCtx->streams[m_videoStreamIdx]->time_base);
        nRet = m_avformatFuncs.av_seek_framePtr(m_formatCtx,m_videoStreamIdx, pts,AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_FRAME);
    }
    else if(m_audioStreamIdx >= 0){
        int64_t pts = sec / r2d(m_formatCtx->streams[m_videoStreamIdx]->time_base);
        nRet = m_avformatFuncs.av_seek_framePtr(m_formatCtx,m_audioStreamIdx, pts,AVSEEK_FLAG_BACKWARD|AVSEEK_FLAG_FRAME);
    }
    return (nRet >= 0)?true:false;
}

AVPacket *MediaDemuxer::readPacket()
{
    CHECK_FORMATCONTEXT(nullptr);

    m_packet->reset();
    AVPacket* pkt = (*m_packet)();
    int nRet = m_avformatFuncs.av_read_framePtr(m_formatCtx, pkt);
    if(nRet!=0)
    {
        return nullptr;
    }

//    //pts/dts turn to ms
//    pkt->pts = pkt->pts*(1000 * (r2d(m_formatCtx->streams[pkt->stream_index]->time_base)));
//    pkt->dts = pkt->dts*(1000 * (r2d(m_formatCtx->streams[pkt->stream_index]->time_base)));

    return pkt;
}

AVPacket *MediaDemuxer::readVideoPacket()
{
    CHECK_FORMATCONTEXT(nullptr);

    for(int i = 0;i<20;++i) {//magic number 20 ,prevent block.
        AVPacket* pkt = readPacket();
        if(pkt && pkt->stream_index == m_videoStreamIdx){
            return pkt;
        }
    }
    return nullptr;
}

AVPacket *MediaDemuxer::readAudioPacket()
{
    CHECK_FORMATCONTEXT(nullptr);

    for(int i = 0;i<20;++i) {//magic number 20 ,prevent block.
        AVPacket* pkt = readPacket();
        if(pkt && pkt->stream_index == m_audioStreamIdx){
            return pkt;
        }
    }
    return nullptr;
}

AVCodecParameters *MediaDemuxer::getVideoPara()
{
    CHECK_FORMATCONTEXT(nullptr);
    if(m_videoStreamIdx < 0){
        return nullptr;
    }
    return m_formatCtx->streams[m_videoStreamIdx]->codecpar;
}

int MediaDemuxer::getVideoFrameIntervalms()
{
    if(m_videoStreamIdx < 0){
        return 0;
    }
    return getFrameIntervalms(m_formatCtx->streams[m_videoStreamIdx]->r_frame_rate.num,
                              m_formatCtx->streams[m_videoStreamIdx]->r_frame_rate.den);
}

double MediaDemuxer::getVideoTimeBaseMs()
{
    if(m_videoStreamIdx < 0){
        return 0;
    }
    return r2d(m_formatCtx->streams[m_videoStreamIdx]->time_base) * 1000;
}

AVCodecParameters *MediaDemuxer::getAudioPara()
{
    CHECK_FORMATCONTEXT(nullptr);
    if(m_audioStreamIdx < 0){
        return nullptr;
    }
    return m_formatCtx->streams[m_audioStreamIdx]->codecpar;
}

double MediaDemuxer::getAudioTimeBaseMs()
{
    if(m_audioStreamIdx < 0){
        return 0;
    }
    return r2d(m_formatCtx->streams[m_audioStreamIdx]->time_base) * 1000;
}

bool MediaDemuxer::updateVideoInfo(AVFormatContext *pFormatcontext)
{
    // 视频 index
    int index = m_avformatFuncs.av_find_best_streamPtr(pFormatcontext,AVMEDIA_TYPE_VIDEO,-1,-1,nullptr,0);
    if(index < 0){
        return false;
    }
    m_videoStreamIdx = index;
    AVStream* as = pFormatcontext->streams[m_videoStreamIdx];
    m_width = as->codecpar->width;
    m_height = as->codecpar->height;
    qDebug()<<"duration:"<<pFormatcontext->streams[m_videoStreamIdx]->duration;
    qDebug()<<"=====================================================";
    qDebug()<<"codec_id: "<<as->codecpar->codec_id;

    //AVSampleFormat
    qDebug()<<"format: "<<as->codecpar->format;
    qDebug()<<"video info: "<< m_videoStreamIdx;
    qDebug()<<"width:"<<as->codecpar->width;
    qDebug()<<"height:"<<as->codecpar->height;
    qDebug()<<"video fps = "<<r2d(as->avg_frame_rate); //avg_frame_rate 为一数组，记录分子分母以保证精度
    return true;
}

bool MediaDemuxer::updateAudioInfo(AVFormatContext *pFormatcontext)
{
    //获取音频 index
    int index = m_avformatFuncs.av_find_best_streamPtr(pFormatcontext,AVMEDIA_TYPE_AUDIO,-1,-1,nullptr,0);
    if(index <0){
        return false;
    }
    m_audioStreamIdx = index;
    AVStream* as = pFormatcontext->streams[m_audioStreamIdx];
    m_sampleRate = as->codecpar->sample_rate;
    m_channels = as->codecpar->channels;
    qDebug()<<"duration:"<<pFormatcontext->streams[m_audioStreamIdx]->duration;
    qDebug()<<"=====================================================";
    qDebug()<<"codec_id: "<<as->codecpar->codec_id;
    qDebug()<<"format: "<<as->codecpar->format;
    qDebug()<<"audio info: "<< m_audioStreamIdx;
    qDebug()<<"sample rate: "<<as->codecpar->sample_rate;
    qDebug()<<"channels: "<<as->codecpar->channels;
    qDebug()<<"frame size:"<<as->codecpar->frame_size; //单通道定量样本数量
    //1024(frame size) * 2(双通道) * format(以8bit为基准) 4 = 8192
    return true;
}
