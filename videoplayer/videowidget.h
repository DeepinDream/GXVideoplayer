#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>

class AVFrame;

class VideoWidget : public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    VideoWidget(QWidget* parent = nullptr);
    ~VideoWidget();
    void init(int nWidth,int nHeight);
    bool repaint(AVFrame *pFrame);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    void _release();
    void _copyNV12(AVFrame *pFrame);
    void _copySameSizeView(AVFrame *pFrame);
    void _copyAlignmentView(AVFrame *pFrame);

private:
    int m_nWidth;
    int m_nHeight;
    QGLShaderProgram m_program;
    GLuint unis[3] = {0};
    GLuint texs[3] = {0};
    unsigned char *datas_y;
    unsigned char *datas_u;
    unsigned char *datas_v;
};

#endif // VIDEOWIDGET_H
