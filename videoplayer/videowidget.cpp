#include "videowidget.h"

extern "C"
{
#include "libavutil/frame.h"
}

//準備yuv數據
//ffmpeg -i input.mp4 -t 10 -s 240x128 -pix_fmt yuv420p out240x128.yuv
//自動加雙引號
#define GET_STR(x) #x
#define A_VER 3
#define T_VER 4

//頂點shader

const char *vString = GET_STR(
    attribute vec4 vertexIn;
    attribute vec2 textureIn;
    varying vec2 textureOut;
    void main(void)
    {
        gl_Position = vertexIn;
        textureOut = textureIn;
    }
);




//像素shader
const char *tString = GET_STR(
        varying vec2 textureOut;
        uniform sampler2D tex_y;
        uniform sampler2D tex_u;
        uniform sampler2D tex_v;
        void main(void)
        {
            vec3 yuv;
            vec3 rgb;
            yuv.x = texture2D(tex_y, textureOut).r;
            yuv.y = texture2D(tex_u, textureOut).r - 0.5;
            yuv.z = texture2D(tex_v, textureOut).r - 0.5;
            rgb = mat3(1.0, 1.0, 1.0,
                0.0, -0.39465, 2.03211,
                1.13983, -0.58060, 0.0) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }

);



VideoWidget::VideoWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_nWidth(0)
    , m_nHeight(0)
    , datas_y(nullptr)
    , datas_u(nullptr)
    , datas_v(nullptr)
{

}

VideoWidget::~VideoWidget()
{
    _release();

    if(texs[0])
    {
        glDeleteTextures(3,texs);
    }
}

void VideoWidget::init(int nWidth, int nHeight)
{
    m_nWidth = nWidth;
    m_nHeight = nHeight;

    _release();

    datas_y = new unsigned char[m_nWidth*m_nHeight];   //Y
    datas_u = new unsigned char[m_nWidth*m_nHeight/4];	//U
    datas_v = new unsigned char[m_nWidth*m_nHeight/4];	//V

    if(texs[0])
    {
        glDeleteTextures(3,texs);
    }
    //創建材質
    glGenTextures(3,texs);
    //Y
    glBindTexture(GL_TEXTURE_2D, texs[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nWidth, m_nHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    //U
    glBindTexture(GL_TEXTURE_2D, texs[1]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nWidth/2, m_nHeight / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    //V
    glBindTexture(GL_TEXTURE_2D, texs[2]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nWidth / 2, m_nHeight / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
}

bool VideoWidget::repaint(AVFrame *pFrame)
{
    if(!pFrame)
        return false;
    if(!datas_y || m_nWidth*m_nHeight == 0||
            pFrame->width != m_nWidth || pFrame->height != m_nHeight)
    {
        return false;
    }

    switch ((AVPixelFormat)pFrame->format)
    {
    case AV_PIX_FMT_YUV420P:
        if(m_nWidth == pFrame->linesize[0])
            _copySameSizeView(pFrame);
        else
            _copyAlignmentView(pFrame);
        break;
    case AV_PIX_FMT_NV12:
    {
        _copyNV12(pFrame);
        break;
    }
    case AV_PIX_FMT_VAAPI_VLD:
        qDebug()<<"frame->data[3]:"<<pFrame->data[3];
        qDebug()<<"AV_PIX_FMT_VAAPI_VLD";
        break;
    default:
        qDebug()<<"undefined format.";
        break;
    }

    update();

    return true;
}

void VideoWidget::initializeGL()
{
    qInfo()<<"initializeGL";
    //初始化opengl函數 由QOpenGLFunctions繼承
    initializeOpenGLFunctions();
    //program加載頂點和像素shader腳本
    qInfo()<<m_program.addShaderFromSourceCode(QGLShader::Fragment,tString);
    qInfo()<<m_program.addShaderFromSourceCode(QGLShader::Vertex,vString);

    //設置頂點座標變量
    m_program.bindAttributeLocation("vertexIn",A_VER);
    //設置材質座標
    m_program.bindAttributeLocation("textureIn",T_VER);


    //編譯shader
    qInfo()<<"program link() = "<<m_program.link();
    qInfo()<<"program bind() = "<<m_program.bind();

    //傳遞頂點和材質座標
    //頂點
    static const GLfloat ver[] = {
        -1.0f,-1.0f,
        1.0f,-1.0f,
        -1.0f,1.0f,
        1.0f,1.0f
    };
    //材質
    static const GLfloat tex[] = {
        0.0f,1.0f,
        1.0f,1.0f,
        0.0f,0.0f,
        1.0f,0.0f
    };
    //頂點
    glVertexAttribPointer(A_VER,2,GL_FLOAT,0,0,ver);
    glEnableVertexAttribArray(A_VER);

    //材質
    glVertexAttribPointer(T_VER,2,GL_FLOAT,0,0,tex);
    glEnableVertexAttribArray(T_VER);

    //從shader獲取材質
    unis[0] = m_program.uniformLocation("tex_y");
    unis[1] = m_program.uniformLocation("tex_u");
    unis[2] = m_program.uniformLocation("tex_v");
}

void VideoWidget::resizeGL(int w, int h)
{
    qInfo()<<"resizeGL"<<w<<" "<<h;
}

void VideoWidget::paintGL()
{
    //y
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texs[0]);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,m_nWidth,m_nHeight,GL_RED,GL_UNSIGNED_BYTE,datas_y);
    glUniform1i(unis[0],0);

    //u
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D,texs[1]);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,m_nWidth/2,m_nHeight/2,GL_RED,GL_UNSIGNED_BYTE,datas_u);
    glUniform1i(unis[1],1);

    //v
    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D,texs[2]);
    glTexSubImage2D(GL_TEXTURE_2D,0,0,0,m_nWidth/2,m_nHeight/2,GL_RED,GL_UNSIGNED_BYTE,datas_v);
    glUniform1i(unis[2],2);

    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

void VideoWidget::_release()
{
    if(datas_y != nullptr){
        delete [] datas_y;
        datas_y = nullptr;
    }

    if(datas_u != nullptr){
        delete [] datas_u;
        datas_u = nullptr;
    }

    if(datas_v != nullptr){
        delete [] datas_v;
        datas_v = nullptr;
    }
}

void VideoWidget::_copyNV12(AVFrame *pFrame)
{
    memcpy(datas_y,pFrame->data[0],m_nWidth*m_nHeight);
    for(int i =0,u=0,v=0;i<m_nWidth*m_nHeight/2;++i)
    {
        if(i%2==0)
            datas_u[u++] = pFrame->data[1][i];
        else
            datas_v[v++] = pFrame->data[1][i];
    }
}

void VideoWidget::_copySameSizeView(AVFrame *pFrame)
{
    memcpy(datas_y,pFrame->data[0],m_nWidth*m_nHeight);
    memcpy(datas_u,pFrame->data[1],m_nWidth*m_nHeight/4);
    memcpy(datas_v,pFrame->data[2],m_nWidth*m_nHeight/4);
}

void VideoWidget::_copyAlignmentView(AVFrame *pFrame)
{
    for(int i =0;i<m_nHeight;++i)//y
        memcpy(datas_y+m_nWidth*i,pFrame->data[0]+pFrame->linesize[0]*i,m_nWidth);
    for(int i =0;i<m_nHeight/2;++i)//u
        memcpy(datas_u+m_nWidth/2*i,pFrame->data[1]+pFrame->linesize[1]*i,m_nWidth);
    for(int i =0;i<m_nHeight/2;++i)//v
        memcpy(datas_v+m_nWidth/2*i,pFrame->data[2]+pFrame->linesize[2]*i,m_nWidth);
}
