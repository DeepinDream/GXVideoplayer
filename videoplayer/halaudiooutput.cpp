#include "halaudiooutput.h"

#include <QAudioFormat>
#include <QAudioOutput>
#include <QDebug>

HalAudioOutput::HalAudioOutput()
    : m_pIO(nullptr)
{

}

HalAudioOutput::~HalAudioOutput()
{
    close();
}

bool HalAudioOutput::open()
{
    close();
    QAudioFormat fmt;
    fmt.setSampleRate(m_sampleRate);
    fmt.setSampleSize(m_sampleSize);
    fmt.setChannelCount(m_channels);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    QAudioFormat PreferInfo = info.preferredFormat();
    qDebug()<<"PreferInfo codec:"<<PreferInfo.codec();
    qDebug()<<"PreferInfo sampleRate"<<PreferInfo.sampleRate();
    qDebug()<<"PreferInfo sampleSize"<<PreferInfo.sampleSize();
    qDebug()<<"PreferInfo sampleType"<<PreferInfo.sampleType();
    qDebug()<<"PreferInfo channelCount"<<PreferInfo.channelCount();
    qDebug()<<"PreferInfo byteOrder"<<PreferInfo.byteOrder();
    for(auto ele:info.supportedSampleSizes())
        qDebug()<<"support samplesizes:"<<ele;
    if (!info.isFormatSupported(fmt))
    {
        qWarning()<<"raw audio format not supported by backend, cannot play audio.";
        fmt = info.nearestFormat(fmt);
        qDebug()<<"fixed codec:"<<fmt.codec();
        qDebug()<<"fixed sampleRate"<<fmt.sampleRate();
        qDebug()<<"fixed sampleSize"<<fmt.sampleSize();
        qDebug()<<"fixed sampleType"<<fmt.sampleType();
        qDebug()<<"fixed channelCount"<<fmt.channelCount();
        qDebug()<<"fixed byteOrder"<<fmt.byteOrder();
    }

    m_pOutput.reset(new QAudioOutput(fmt));
    m_pIO = m_pOutput->start();
    qDebug()<<"error:"<<m_pOutput->error();
    return (m_pIO != nullptr) ? true :false;
}

void HalAudioOutput::close()
{
    if(m_pIO)
    {
        m_pIO->close();
        m_pIO = nullptr;
    }
    if(m_pOutput)
    {
        m_pOutput->stop();
    }
}

void HalAudioOutput::clear()
{
    if(m_pIO)
    {
        m_pIO->reset();
    }
}

bool HalAudioOutput::write(const unsigned char *pData, int nDataSize)
{
    if(!pData || nDataSize<=0)
        return false;

    if(!m_pIO || !m_pOutput)
    {
        return false;
    }
    int nSize = m_pIO->write((char*)pData,nDataSize);
    if(nDataSize != nSize)
        return false;

    return true;
}

int HalAudioOutput::getFree()
{
    if(!m_pOutput)
    {
        return 0;
    }
    int nFree = m_pOutput->bytesFree();
    return nFree;
}

long long HalAudioOutput::getNoPlayMs()
{
    if(!m_pOutput)
    {
        return 0;
    }
    long long nPts = 0;
    //还未播放的字节数大小
    double dSize = m_pOutput->bufferSize() - m_pOutput->bytesFree();
    //一秒音频大小 samplesize 以 8 为一字节(byte)
    double dSecPerSize = m_sampleRate * (m_sampleSize/8) * m_channels;
    if(dSecPerSize<=0)
        nPts = 0;
    else
    {
        nPts = (dSize/dSecPerSize)*1000; //turn to ms
    }
    return nPts;
}

void HalAudioOutput::setPause(bool bIsPause)
{
    if(!m_pOutput)
    {
        return;
    }
    if(bIsPause)
    {
        m_pOutput->suspend();
    }
    else
    {
        m_pOutput->resume();
    }
}
