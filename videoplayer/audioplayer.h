#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <QString>

class IHalAudioOutput;
class AudioResample;
class MediaDemuxer;
class MediaDecoder;

struct AVCodecParameters;

class AudioPlayer
{
public:
    AudioPlayer();
    ~AudioPlayer();

    bool open(const QString& strUrl);
    void start();
    void setplay(const bool playing);
    bool playing() const;
    void stop();
    void clear();
    void seek(const double pos);

    long long getPts();
    long long getTotalMs() const;
private:
    bool _open(AVCodecParameters* pPara,int nSampleRate,int nChannels);
    void _run();
private:
    std::unique_ptr<MediaDemuxer> m_demuxer;
    std::unique_ptr<MediaDecoder> m_decoder;
    std::unique_ptr<AudioResample> m_resample;
    std::unique_ptr<IHalAudioOutput> m_audioOutput;
    long long m_pts;

    std::unique_ptr<std::thread> m_thread;
    std::atomic_bool m_running;
    bool m_playing;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    double m_timeBase;
};

#endif // AUDIOPLAYER_H

