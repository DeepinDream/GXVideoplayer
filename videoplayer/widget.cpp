#include "widget.h"
#include "ui_widget.h"
#include "videorender.h"
#include "mediaplayer.h"

#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ConnectUI();

    m_pVideoCall = new VideoRender(ui->Video);
    m_player = std::make_unique<MediaPlayer>();

    startTimer(30);
}

Widget::~Widget()
{
    delete m_pVideoCall;
    delete ui;
}

void Widget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    if(!m_player->playing()){
        SetPlay(false);
        return;
    }
    if(ui->horizontalSlider->pressed()){
        return;
    }

    auto totalTime = m_player->totalMs();
    auto pts = m_player->pts();
    if(totalTime <= 0 || llabs(pts) > totalTime){
        return;
    }

    auto pos = (double)pts / (double)totalTime;
    int v = ui->horizontalSlider->maximum() * pos;
    ui->horizontalSlider->setValue(v);
}

void Widget::SetPlay(bool isPlay)
{
    if(m_player->playing() == isPlay){
        return;
    }
    if(isPlay)
        ui->BtnStart->setText("Pause");
    else
        ui->BtnStart->setText("Play");

    if(m_player->playing() != isPlay){
        m_player->setplay(isPlay);
    }
}

void Widget::HandleOpenFile()
{
    QString strFileName = QFileDialog::getOpenFileName(nullptr,QString::fromLocal8Bit("選擇檔案："));
    if(strFileName.isEmpty())
        return;
    this->setWindowTitle(strFileName);

    if(!m_player->Open(strFileName.toStdString().c_str(), m_pVideoCall)){
        QMessageBox::information(nullptr,"error","open file failed!");
        return;
    }

    ui->BtnStart->setChecked(true);
    SetPlay(true);
    m_player->start();
}

void Widget::closeEvent(QCloseEvent* event)
{
    SetPlay(false);
    m_player->stop();
}

void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    if(isFullScreen())
        this->showNormal();
    else
        this->showFullScreen();
}

void Widget::ConnectUI()
{
    connect(ui->pushButton,&QPushButton::clicked,this,&Widget::HandleOpenFile);
    connect(ui->horizontalSlider, &MySlider::sgValueChangeByUser, this, &Widget::on_valueChangedByUser);
}

void Widget::on_BtnStart_clicked(bool checked)
{
    SetPlay(checked);
}

void Widget::on_valueChangedByUser(int value)
{
    double maximum = (double)ui->horizontalSlider->maximum();
    double pos = value / maximum;
    m_player->seek(pos);
}
