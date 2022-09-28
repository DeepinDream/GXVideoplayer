#ifndef HALAUDIOOUTPUT_H
#define HALAUDIOOUTPUT_H

#include "IHalAudioOutput.h"
#include <memory>

class QAudioOutput;
class QIODevice;

class HalAudioOutput: public IHalAudioOutput
{
public:
    HalAudioOutput();
    ~HalAudioOutput();
    virtual bool open() override;
    virtual void close() override;
    virtual void clear() override;
    virtual bool write(const unsigned char* pData,int nDataSize) override;
    virtual int getFree() override;
    virtual long long getNoPlayMs() override; //缓存中未播放的时间 ms
    virtual void setPause(bool bIsPause) override;

private:
    std::unique_ptr<QAudioOutput> m_pOutput;
    QIODevice* m_pIO;
};

#endif // HALAUDIOOUTPUT_H
