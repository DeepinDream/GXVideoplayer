#include "mediaplayer.h"
#include "ffmpegwrapper/ffmpegwrapper.h"
#include "audioplayer.h"
#include "videoplayer.h"

#include <QDir>
#include <QDebug>
#include <QApplication>

MediaPlayer::MediaPlayer()
    : m_playing(false)
{
    FFMPegWrapper::instance().init(qApp->applicationDirPath());
}

MediaPlayer::~MediaPlayer()
{
}

bool MediaPlayer::Open(const char *strUrl, IVideoCall *pVideoCall)
{
    if(strUrl == nullptr || strUrl[0]=='\0')
        return false;

    m_audioPlayer.reset(new AudioPlayer);
    bool audioOk = m_audioPlayer->open(strUrl);

    //return audioOk;

    m_videoPlayer.reset(new VideoPlayer);
    bool videoOk = m_videoPlayer->open(strUrl, pVideoCall);

    if(audioOk && videoOk){
        m_audioPlayer->setSyncPtsCallback([this](long long pts){
           m_videoPlayer->setSyncPts(pts);
        });
    }
    return audioOk || videoOk;
}

void MediaPlayer::start()
{
    if(m_audioPlayer){
        m_audioPlayer->start();
    }
    if(m_videoPlayer){
        m_videoPlayer->start();
    }
}

void MediaPlayer::setplay(const bool playing)
{
    if(m_audioPlayer){
        m_audioPlayer->setplay(playing);
    }
    if(m_videoPlayer){
        m_videoPlayer->setplay(playing);
    }
}

void MediaPlayer::stop()
{
    if(m_audioPlayer){
        m_audioPlayer->stop();
    }
    if(m_videoPlayer){
        m_videoPlayer->stop();
    }
}

long long MediaPlayer::totalMs() const
{
    if(m_audioPlayer){
        return m_audioPlayer->getTotalMs();
    }
    if(m_videoPlayer){
        return m_videoPlayer->getTotalMs();
    }
    return 0;
}

bool MediaPlayer::playing() const
{
    if(m_audioPlayer){
        return m_audioPlayer->playing();
    }
    if(m_videoPlayer){
        return m_videoPlayer->playing();
    }
    return false;
}

long long MediaPlayer::pts() const
{
    if(m_audioPlayer){
        return m_audioPlayer->getPts();
    }
    if(m_videoPlayer){
        return m_videoPlayer->getPts();
    }
    return 0;
}

void MediaPlayer::seek(const double pos)
{
    if(m_audioPlayer){
        m_audioPlayer->seek(pos);
    }
    if(m_videoPlayer){
        m_videoPlayer->seek(pos);
    }
}
