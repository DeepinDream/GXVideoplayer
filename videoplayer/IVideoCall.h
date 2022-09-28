#ifndef IVIDEOCALL_H
#define IVIDEOCALL_H

struct AVFrame;
class IVideoCall
{
public:
    virtual ~IVideoCall(){}
    virtual void init(int nWidth, int nHeight) = 0;
    virtual void repaint(AVFrame* pFrame) = 0;
};


#endif // IVIDEOCALL_H
