#include "audioplayer.h"
#include "mediademuxer.h"
#include "mediadecoder.h"
#include "audioresample.h"
#include "halaudiooutput.h"
#include "threadconfig.h"
#include <QDebug>

AudioPlayer::AudioPlayer()
    : m_pts(0)
    , m_running(false)
    , m_playing(false)
    , m_timeBase(0)
{
    m_resample = std::make_unique<AudioResample>();
    m_audioOutput = std::make_unique<HalAudioOutput>();
}

AudioPlayer::~AudioPlayer()
{
    stop();
}

bool AudioPlayer::open(const QString &strUrl)
{
    m_demuxer.reset(new MediaDemuxer);

    if(!m_demuxer->open(strUrl)){
        qDebug()<<"m_pDemux->Open(strUrl) failed.";
        return false;
    }

    return _open(m_demuxer->getAudioPara(), m_demuxer->getSampleRate(), m_demuxer->getChannels());
}

bool AudioPlayer::_open(AVCodecParameters *pPara, int nSampleRate, int nChannels)
{
    if(!pPara)
        return false;

    m_decoder.reset(new MediaDecoder);
    m_pts = 0;

    if(!m_resample->open(pPara))
    {
        qDebug()<<"AudioPlayer open failed.";
        return false;
    }

    m_audioOutput->setSampleRate(nSampleRate);
    m_audioOutput->setChannels(nChannels);
    if(!m_audioOutput->open())
    {
        qDebug()<<"AudioOutput open failed.";
        return false;
    }
    if(!m_decoder->open(pPara)){
        qDebug()<<"Audio Decoder open failed.";
        return false;
    }

    m_timeBase = m_demuxer->getAudioTimeBaseMs();
    qDebug()<<"Audio Player open:"<< true;

    return true;
}

void AudioPlayer::start()
{
    setplay(true);
    if(!m_thread){
        m_running = true;
        m_thread = std::make_unique<std::thread>(std::bind(&AudioPlayer::_run, this));
    }
}

void AudioPlayer::setplay(const bool playing)
{
    std::lock_guard<std::mutex> lk(m_mutex);
    m_playing = playing;
    m_cv.notify_one();
}

bool AudioPlayer::playing() const
{
    return m_playing;
}

void AudioPlayer::stop()
{
    m_running = false;
    m_timeBase = 0;
    setplay(false);
    if(m_thread && m_thread->joinable()){
        m_thread->join();
    }
    clear();
}

void AudioPlayer::clear()
{
    m_decoder->clear();
    m_audioOutput->clear();
}

void AudioPlayer::seek(const double pos)
{
    bool playing = m_playing;
    if(m_demuxer){
        if(playing){
            setplay(false);
        }
        m_demuxer->seek(pos);
        if(playing){
            setplay(true);
        }

    }
}

void AudioPlayer::setSyncPtsCallback(std::function<void (long long)> callback)
{
    m_syncPtsCallback = callback;
}

long long AudioPlayer::getPts()
{
    return m_pts;
}

long long AudioPlayer::getTotalMs() const
{
    if(m_demuxer){
        return m_demuxer->getTotalTimeMs();
    }
    return 0;
}

void AudioPlayer::_run()
{
    SetThreadName("AudioPlayerThread");
    unsigned char* pcm = new unsigned char[1024*1024*10];

    while (m_running) {
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_cv.wait(lk, [this]{return m_playing || !m_running;});
            if(!m_running){
                return;
            }
        }

        AVFrame* pFrame = nullptr;
        do{
            AVPacket* pck = m_demuxer->readAudioPacket();
            MediaDecoder::DecodeError ret = m_decoder->decodePktToFrame(pck, &pFrame);
            if(ret == MediaDecoder::DecodeError::ERROR_RECIVE_EOF){
                m_playing = false;
                break;
            }
            if(ret != MediaDecoder::DecodeError::SUCCESS){
                break;
            }
        }while(false);

        //减去缓存中未播放的时间 单位ms
        m_pts = (m_decoder->getPts() * m_timeBase) - m_audioOutput->getNoPlayMs();

        if(m_syncPtsCallback){
            m_syncPtsCallback(m_pts);
        }
        //resample
        int nSize = m_resample->resample(pFrame,pcm);
        //  release frame
        FFmpegFreeFrame(&pFrame);

        //play audio
        while(m_playing)
        {
            if(nSize<=0)
                break;
            int nFree = m_audioOutput->getFree();
            if(nFree<nSize)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            m_audioOutput->write(pcm,nSize);
            break;
        }
    }

    delete []pcm;
}
