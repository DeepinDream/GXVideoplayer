#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <QString>

class IVideoCall;
class MediaDemuxer;
class MediaDecoder;

struct AVCodecParameters;

class VideoPlayer
{
public:
    VideoPlayer();
    ~VideoPlayer();

    bool open(const char *strUrl, IVideoCall *pVideoCall);
    void start();
    void setplay(const bool playing);
    bool playing() const;
    void stop();
    void clear();
    void seek(const double pos);
    void setSyncPts(long long pts);

    long long getPts();
    long long getTotalMs() const;
private:
    bool _open(AVCodecParameters* pPara, IVideoCall *pVideoCall, int width, int height, int frameIntervalms);
    void _run();
private:
    std::unique_ptr<MediaDemuxer> m_demuxer;
    std::unique_ptr<MediaDecoder> m_decoder;
    IVideoCall* m_videoCall;
    long long m_pts;
    int m_frameIntervalms;
    std::atomic_llong m_syncPts; //同步时间，由外部传入
    const int m_cumulaMax = 64;
    double m_timeBaseMs;
    int m_frameIntervalmsReal;

    std::unique_ptr<std::thread> m_thread;
    std::atomic_bool m_running;
    bool m_playing;
    std::mutex m_mutex;
    std::condition_variable m_cv;
};

#endif // VIDEOPLAYER_H
