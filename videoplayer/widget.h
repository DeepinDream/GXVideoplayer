#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class FFmpegVideoWidget;
class IVideoCall;
class MediaPlayer;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void timerEvent(QTimerEvent *event) override;
    void SetPlay(bool bIsPause);
public slots:
    void HandleOpenFile();
protected:
    void closeEvent(QCloseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    void on_BtnStart_clicked(bool checked);

    void on_valueChangedByUser(int value);

private:
    void ConnectUI();
    void _setTimeElapsedText(const long long ptsMs, const long long totalMs);

private:
    Ui::Widget *ui;
    std::unique_ptr<MediaPlayer> m_player;
    IVideoCall* m_pVideoCall;
};
#endif // WIDGET_H
