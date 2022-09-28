#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QSlider>

class MySlider : public QSlider
{
    Q_OBJECT
public:
    explicit MySlider(QWidget *parent = nullptr);

    bool pressed() const;

    void setPressed();

    void setReleased();

signals:
    void sgValueChangeByUser(int value);
protected:
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;

private slots:
    void onValueChanged(int value);

private:
    bool m_pressed;
};

#endif // MYSLIDER_H
