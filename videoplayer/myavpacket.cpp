#include "myavpacket.h"
#include "ffmpegwrapper/ffmpegdllfuncdef.h"
#include "ffmpegwrapper/ffmpegwrapper.h"

void FFmpegFreePacket(AVPacket** pPkt)
{
    if(!pPkt || !(*pPkt))
        return;
    FFMPegWrapper::instance().get_avcodec_ExportFuncs().av_packet_freePtr(pPkt);
}

MyAVPacket::MyAVPacket()
{
    m_packet = FFMPegWrapper::instance().get_avcodec_ExportFuncs().av_packet_allocPtr();
}

MyAVPacket::~MyAVPacket()
{
    FFMPegWrapper::instance().get_avcodec_ExportFuncs().av_packet_unrefPtr(m_packet);
    FFmpegFreePacket(&m_packet);
}
