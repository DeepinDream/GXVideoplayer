#include "mediadecoder.h"
#include "ffmpegwrapper/ffmpegwrapper.h"

#include <QDebug>

static double r2d(AVRational r)
{
    return r.den == 0 ? 0 :static_cast<double>(r.num)/static_cast<double>(r.den);
}

void FFmpegFreeFrame(AVFrame** pFrame)
{
    if(!pFrame || !(*pFrame))
        return;
    FFMPegWrapper::instance().get_avutil_ExportFuncs().av_frame_freePtr(pFrame);
}

MediaDecoder::MediaDecoder()
    :m_pCodecContext(nullptr)
    ,m_pBufferRef(nullptr)
    ,m_pts(0)
{
    m_avcodecFuncs = FFMPegWrapper::instance().get_avcodec_ExportFuncs();
    m_avutilFuncs = FFMPegWrapper::instance().get_avutil_ExportFuncs();
}

MediaDecoder::~MediaDecoder()
{
    close();
}

bool MediaDecoder::open(AVCodecParameters *pPara)
{
    if(!pPara)
        return false;
    close();

    // 找到解码器
    AVCodec* pCodec = m_avcodecFuncs.avcodec_find_decoderPtr(pPara->codec_id);
    if(!pCodec)
    {
        m_avcodecFuncs.avcodec_parameters_freePtr(&pPara);
        qDebug()<<"can't find avcodec decoder:"<<pPara->codec_id;
        return false;
    }
    qDebug()<<"find avcodec decoder:"<<pPara->codec_id;

    m_pCodecContext = m_avcodecFuncs.avcodec_alloc_context3Ptr(pCodec);
    //配置codec context 参数
    m_avcodecFuncs.avcodec_parameters_to_contextPtr(m_pCodecContext,pPara);
    m_pCodecContext->thread_count = 8; //8 threads decode

    int ret = m_avcodecFuncs.avcodec_open2Ptr(m_pCodecContext,pCodec,nullptr); //check

    if(ret != 0)
    {
        m_avcodecFuncs.avcodec_free_contextPtr(&m_pCodecContext);
        char buf[1024] = {0};
        m_avutilFuncs.av_strerrorPtr(ret,buf,sizeof(buf)-1);
        qDebug()<<"avcodec_open2 fail:"<<buf;
        return false;
    }

    qDebug()<<"avcodec_open2 success";
    return true;
}

void MediaDecoder::close()
{
    if(m_pCodecContext)
    {
        m_avcodecFuncs.avcodec_free_contextPtr(&m_pCodecContext);
    }
    if(m_pBufferRef)
        m_avutilFuncs.av_buffer_unrefPtr(&m_pBufferRef);
    m_pts = 0;
}

void MediaDecoder::clear()
{
    if(m_pCodecContext)
        m_avcodecFuncs.avcodec_flush_buffersPtr(m_pCodecContext);
}

MediaDecoder::DecodeError MediaDecoder::decodePktToFrame(AVPacket *pInPacket, AVFrame **ppOutFrame)
{
    return _decodePktToFrame(m_pCodecContext, pInPacket, ppOutFrame);
}

int64_t MediaDecoder::getPts() const
{
    return m_pts;
}

MediaDecoder::DecodeError MediaDecoder::_decodePktToFrame(AVCodecContext *pDecoderCtx, AVPacket *pInPacket, AVFrame **ppOutFrame)
{
    AVFrame*  pOutFrame = nullptr;
    int res = 0;

    // 送入要解码的数据包
    res = m_avcodecFuncs.avcodec_send_packetPtr(pDecoderCtx, pInPacket);
    if (res == AVERROR(EAGAIN)) // 没有数据送入,但是可以继续可以从内部缓冲区读取编码后的视频包
    {
        //    qDebug("<DecodePktToFrame> avcodec_send_frame() EAGAIN\n");
    }
    else if (res == AVERROR_EOF) // 数据包送入结束不再送入,但是可以继续可以从内部缓冲区读取编码后的视频包
    {
        //    qDebug("<DecodePktToFrame> avcodec_send_frame() AVERROR_EOF\n");
    }
    else if (res < 0)  // 送入输入数据包失败
    {
        qDebug("<DecodePktToFrame> [ERROR] fail to avcodec_send_frame(), res=%d\n", res);
        return DecodeError::ERROR_SEND_FAIL;
    }


    // 获取解码后的视频帧或者音频帧
    pOutFrame = m_avutilFuncs.av_frame_allocPtr();
    res       = m_avcodecFuncs.avcodec_receive_framePtr(pDecoderCtx, pOutFrame);
    m_pts = (pOutFrame->pts>=INT_MAX || pOutFrame->pts <=INT_MIN)?0:pOutFrame->pts;

    if (res == AVERROR(EAGAIN)) // 当前这次没有解码后的音视频帧输出,但是后续可以继续读取
    {
        qDebug("<DecodePktToFrame> no data output\n");
        m_avutilFuncs.av_frame_freePtr(&pOutFrame);
        (*ppOutFrame) = nullptr;
        return DecodeError::ERROR_RECIVE_EAGAIN;
    }
    else if (res == AVERROR_EOF) // 解码缓冲区已经刷新完成,后续不再有数据输出
    {
        qDebug("<DecodePktToFrame> avcodec_receive_packet() EOF\n");
        m_avutilFuncs.av_frame_freePtr(&pOutFrame);
        (*ppOutFrame) = nullptr;
        return DecodeError::ERROR_RECIVE_EOF;
    }
    else if (res < 0)
    {
        qDebug("<DecodePktToFrame> [ERROR] fail to avcodec_receive_packet(), res=%d\n", res);
        m_avutilFuncs.av_frame_freePtr(&pOutFrame);
        (*ppOutFrame) = nullptr;
        return DecodeError::ERROR_RECIVE_OTHER;
    }

    (*ppOutFrame) = pOutFrame;
    return DecodeError::SUCCESS;
}
