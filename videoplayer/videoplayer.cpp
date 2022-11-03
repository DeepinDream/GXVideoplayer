#include "videoplayer.h"
#include "mediademuxer.h"
#include "mediadecoder.h"
#include "audioresample.h"
#include "halaudiooutput.h"
#include "threadconfig.h"
#include "IVideoCall.h"
#include <QDebug>
#include <QElapsedTimer>

void LimitInterval(int &v)
{ // pts <= 50，限制帧率不要太高
    v = (v >= 20 ? v : 20);
}

VideoPlayer::VideoPlayer()
    : m_videoCall(NULL)
    , m_pts(0)
    , m_frameIntervalms(0)
    , m_running(false)
    , m_playing(false)
    , m_frameIntervalmsReal(0)
{

}

VideoPlayer::~VideoPlayer()
{
    stop();
}

bool VideoPlayer::open(const char *strUrl, IVideoCall *pVideoCall)
{
    m_demuxer.reset(new MediaDemuxer);

    if(!m_demuxer->open(strUrl)){
        qDebug()<<"Video m_pDemux->Open(strUrl) failed.";
        return false;
    }

    return _open(m_demuxer->getVideoPara(), pVideoCall, m_demuxer->getWidth(), m_demuxer->getHeight(), m_demuxer->getVideoFrameIntervalms());
}

bool VideoPlayer::_open(AVCodecParameters *pPara, IVideoCall *pVideoCall, int width, int height, int frameIntervalms)
{
    if(!pPara)
        return false;

    m_videoCall = pVideoCall;
    m_decoder.reset(new MediaDecoder);
    m_pts = 0;
    m_frameIntervalms = frameIntervalms;
    m_frameIntervalmsReal = m_frameIntervalms;
    if(!m_decoder->open(pPara)){
        qDebug()<<"Video Decoder open failed.";
        return false;
    }

    if(m_videoCall){
        m_videoCall->init(width, height);
    }

    m_timeBaseMs = m_demuxer->getVideoTimeBaseMs();

    qDebug()<<"Video Player open:"<< true << ", frameIntervalms: " << frameIntervalms;

    return true;
}

void VideoPlayer::start()
{
    setplay(true);
    if(!m_thread){
        m_running = true;
        m_thread = std::make_unique<std::thread>(std::bind(&VideoPlayer::_run, this));
    }
}

void VideoPlayer::setplay(const bool playing)
{
    std::lock_guard<std::mutex> lk(m_mutex);
    m_playing = playing;
    m_cv.notify_one();
}

bool VideoPlayer::playing() const
{
    return m_playing;
}

void VideoPlayer::stop()
{
    m_running = false;
    setplay(false);
    if(m_thread && m_thread->joinable()){
        m_thread->join();
    }
    clear();
}

void VideoPlayer::clear()
{
    m_decoder->clear();
    m_frameIntervalms = 0;
    m_pts = 0;
}

void VideoPlayer::seek(const double pos)
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

void VideoPlayer::setSyncPts(long long pts)
{
    m_syncPts = pts;
}

long long VideoPlayer::getPts()
{
    return m_pts;
}

long long VideoPlayer::getTotalMs() const
{
    if(m_demuxer){
        return m_demuxer->getTotalTimeMs();
    }
    return 0;
}

void VideoPlayer::_run()
{
    SetThreadName("VideoPlayerThread");
    unsigned char* pcm = new unsigned char[1024*1024*10];
    int framCount = 0;
    long long prePts = 0;
    QElapsedTimer elapsdTimer;

    auto syncPtsAloneFunc = [&, this]{
        framCount++;
        if(framCount == 1){
            prePts = m_pts;
            elapsdTimer.start();
            //            qDebug() << "framCount: " << framCount << ", Video frame sleep: " << m_frameIntervalmsReal;
        }
        else if(framCount >= m_cumulaMax){
            auto total = framCount - 1;
            auto ms = elapsdTimer.restart() / total;
            long long syncPts = m_syncPts;
            long long pts = m_pts * m_timeBaseMs;
            if(syncPts > 0 && syncPts < pts){
                int newIntervalms = (pts - syncPts) / total * 2;
                LimitInterval(newIntervalms);
                m_frameIntervalmsReal = newIntervalms;
                qDebug() << "framCount: " << framCount << ", total: " << total
                         << ", pts: " << pts
                         << ", syncPts: " << syncPts
                         << ", newIntervalms: " << newIntervalms;
            }
            else{
                auto avg = (m_pts - prePts) * m_timeBaseMs / total;
                int newIntervalms = m_frameIntervalmsReal;
                if(m_pts > prePts){
                    newIntervalms = m_frameIntervalmsReal - (ms - avg) / 2;
                }
                LimitInterval(newIntervalms);
                m_frameIntervalmsReal = newIntervalms;
                //            qDebug() << "framCount: " << framCount << ", total: " << total << ", ms: " << ms << ", ms: " << ms
                //                     << ", m_pts: " << m_pts
                //                     << ", prePts: " << prePts
                //                     << ", m_timeBaseMs: " << m_timeBaseMs
                //                     << ", avg: " << avg
                //                     << ", newIntervalms: " << newIntervalms
                //                     << ", Video frame sleep: " << m_frameIntervalmsReal;
            }
            framCount = 0;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(m_frameIntervalmsReal));
    };

    auto syncPtsByAudio = [&, this](){
        long long syncPts = m_syncPts;
        long long pts = m_pts * m_timeBaseMs;
        if(syncPts > 0 && syncPts < pts){
            std::this_thread::sleep_for(std::chrono::milliseconds(pts - syncPts));
        }
    };

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
            AVPacket* pck = m_demuxer->readVideoPacket();
            MediaDecoder::DecodeError ret = m_decoder->decodePktToFrame(pck, &pFrame);
            if(ret == MediaDecoder::DecodeError::ERROR_RECIVE_EOF){
                m_playing = false;
                break;
            }
            if(ret != MediaDecoder::DecodeError::SUCCESS){
                break;
            }
        }while(false);

        m_pts = m_decoder->getPts();
        if(m_videoCall){
            m_videoCall->repaint(pFrame);
        }

        //  release frame
        FFmpegFreeFrame(&pFrame);

                syncPtsAloneFunc();
//        syncPtsByAudio();
    }

    delete []pcm;
}
