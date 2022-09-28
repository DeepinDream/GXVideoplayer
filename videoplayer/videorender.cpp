#include "videorender.h"
#include "videowidget.h"

VideoRender::VideoRender(VideoWidget *_ui)
    :ui(_ui)
{

}

VideoRender::~VideoRender()
{

}

void VideoRender::init(int nWidth, int nHeight)
{
    if(ui){
        ui->init(nWidth, nHeight);
    }
}

void VideoRender::repaint(AVFrame *pFrame)
{
    if(ui){
        ui->repaint(pFrame);
    }
}
