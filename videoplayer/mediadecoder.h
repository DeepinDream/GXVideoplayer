#ifndef MEDIADECODER_H
#define MEDIADECODER_H

#include <stdint.h>
#include "ffmpegwrapper/ffmpegdllfuncdef.h"

extern void FFmpegFreeFrame(AVFrame** pFrame);

struct AVCodecParameters;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;
struct AVBufferRef;

class MediaDecoder
{
public:
    enum DecodeError{
        SUCCESS = 0,
        ERROR_SEND_FAIL = -1,
        ERROR_RECIVE_EAGAIN = -2,
        ERROR_RECIVE_EOF = -3,
        ERROR_RECIVE_OTHER = -4,
    };
    MediaDecoder();
    ~MediaDecoder();
    bool open(AVCodecParameters* pPara);    // 打开解码器
    bool openAudio(AVCodecParameters* pPara, int nSampleRate, int nChannels);

    void close();
    void clear();

    // 功能: 送入一个数据包进行解码,获取解码后的音视频帧
    //      注意: 即使正常解码情况下,不是每次都有解码后的帧输出的
    // 参数: pInPacket   ---- 输入的数据包, 可以为nullptr来刷新解码缓冲区
    //      ppOutFrame  ---- 输出解码后的音视频帧, 即使返回0也可能无输出
    // 返回: 0:           解码正常;
    //      AVERROR_EOF: 解码全部完成;
    //      other:       解码出错
    DecodeError decodePktToFrame(AVPacket* pInPacket, AVFrame** ppOutFrame);

    int64_t getPts() const;

private:
    DecodeError _decodePktToFrame(AVCodecContext* pDecoderCtx, AVPacket* pInPacket, AVFrame** ppOutFrame);
private:
    AVCodecContext* m_pCodecContext;
    AVBufferRef* m_pBufferRef;
    int64_t m_pts;

    C_avcodec_ExportFuncs m_avcodecFuncs;
    C_avutil_ExportFuncs m_avutilFuncs;
};

#endif // MEDIADECODER_H
