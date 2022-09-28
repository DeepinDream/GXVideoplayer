#ifndef IHALAUDIOOUTPUT_H
#define IHALAUDIOOUTPUT_H

// 音频输出硬件虚拟层
class IHalAudioOutput
{
public:
    IHalAudioOutput() :m_sampleRate(48000), m_sampleSize(16), m_channels(2){}
    virtual ~IHalAudioOutput(){}
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual void clear() = 0;
    virtual bool write(const unsigned char* pData,int nDataSize) = 0;
    virtual int getFree() = 0;
    virtual long long getNoPlayMs() = 0; //缓存中未播放的时间 ms
    virtual void setPause(bool bIsPause) = 0;

    int getSampleRate() const{return m_sampleRate;}
    void setSampleRate(int sampleRate){m_sampleRate = sampleRate;}

    int getSampleSize() const{return m_sampleSize;}
    void setSampleSize(int sampleSize){m_sampleSize = sampleSize;}

    int getChannels() const{return m_channels;}
    void setChannels(int channels){m_channels = channels;}

protected:
    int m_sampleRate;
    int m_sampleSize;
    int m_channels;
};

#endif // IHALAUDIOOUTPUT_H












