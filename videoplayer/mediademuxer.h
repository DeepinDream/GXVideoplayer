#ifndef MEDIADEMUXER_H
#define MEDIADEMUXER_H

#include <QString>

#include "ffmpegwrapper/ffmpegdllfuncdef.h"

struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;
class MyAVPacket;

class MediaDemuxer
{
public:
    MediaDemuxer();
    ~MediaDemuxer();

    bool open(const QString& strUrl);
    void close();

    //seek位置 0.0~1.0
    bool seek(double dPos);

    AVPacket* readPacket();
    AVPacket* readVideoPacket();
    AVPacket* readAudioPacket();

    AVCodecParameters *getVideoPara();
    int getVideoFrameIntervalms();
    double getVideoTimeBaseMs();
    AVCodecParameters *getAudioPara();
    double getAudioTimeBaseMs();

    int getTotalTimeMs() const{return m_totalTimeMs;}
    int getWidth() const{return m_width;}
    int getHeight() const{return m_height;}
    int getSampleRate() const{return m_sampleRate;}
    int getChannels() const{return m_channels;}


private:
    bool updateVideoInfo(AVFormatContext *pFormatcontext);
    bool updateAudioInfo(AVFormatContext* pFormatcontext);

private:
    AVFormatContext* m_formatCtx;
    MyAVPacket* m_packet;
    int m_videoStreamIdx;
    int m_audioStreamIdx;
    int m_totalTimeMs;
    int m_width;
    int m_height;
    int m_sampleRate;
    int m_channels;

    C_avutil_ExportFuncs m_avutilFuncs;
    C_avformat_ExportFuncs m_avformatFuncs;
};

#endif // MEDIADEMUXER_H
