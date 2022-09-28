#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <thread>
#include <memory>
#include <atomic>
#include <mutex>

class IVideoCall;
class AudioPlayer;
class VideoPlayer;

class MediaPlayer
{
public:
    enum MediaType{
        NONE = 0,
        VIDEO = 1,
        AUDIO = 2,
        VIDEO_AUDIO = 3,
    };

    MediaPlayer();
    ~MediaPlayer();

    bool Open(const char* strUrl,IVideoCall* pVideoCall);
    void start();
    void setplay(const bool playing);
    void stop();

    long long totalMs() const;

    bool playing() const;

    long long pts() const;

    void seek(const double pos);

private:
    std::atomic_bool m_running;
    std::mutex m_mutex;
    MediaType m_mediaType;
    long long m_totalMs;
    long long m_pts;
    bool m_playing;

    std::unique_ptr<AudioPlayer> m_audioPlayer;
    std::unique_ptr<VideoPlayer> m_videoPlayer;
};

#endif // MEDIAPLAYER_H
