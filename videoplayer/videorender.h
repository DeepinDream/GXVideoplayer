#ifndef VIDEORENDER_H
#define VIDEORENDER_H

#include "IVideoCall.h"

class VideoWidget;

class VideoRender: public IVideoCall
{
public:
    VideoRender(VideoWidget* _ui);
    virtual ~VideoRender();

    virtual void init(int nWidth, int nHeight) override;
    virtual void repaint(AVFrame* pFrame) override;

private:
    VideoWidget* ui;
};

#endif // VIDEORENDER_H
