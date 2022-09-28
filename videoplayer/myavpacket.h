#ifndef MYAVPACKET_H
#define MYAVPACKET_H

struct AVPacket;

extern void FFmpegFreePacket(AVPacket** pPkt);

class MyAVPacket
{
public:
    MyAVPacket();
    ~MyAVPacket();
    AVPacket* operator()(){return m_packet;}

private:
    AVPacket* m_packet;
};

#endif // MYAVPACKET_H
