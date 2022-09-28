#include "myslider.h"

#include <QMouseEvent>

MySlider::MySlider(QWidget *parent)
    : QSlider(parent)
    , m_pressed(false)
{
//    connect(this, &QSlider::sliderPressed, this, &MySlider::setPressed);
//    connect(this, &QSlider::sliderReleased, this, &MySlider::setReleased);

    connect(this, &QSlider::valueChanged, this, &MySlider::onValueChanged);
}

bool MySlider::pressed() const
{
    return m_pressed;
}

void MySlider::setPressed()
{
    m_pressed = true;
}

void MySlider::setReleased()
{
    m_pressed = false;
}

void MySlider::mousePressEvent(QMouseEvent *ev)
{
    m_pressed = true;
    int x = ev->x();
    int width = this->width();
    auto pos = (double)x / (double)width;
    int v = this->maximum() * pos;
    this->setValue(v);
    QSlider::mousePressEvent(ev);
}

void MySlider::mouseReleaseEvent(QMouseEvent *ev)
{
    if(!pressed())
        return;
    int x = ev->x();
    int width = this->width();
    auto pos = (double)x / (double)width;
    int value = this->maximum() * pos;
    emit sgValueChangeByUser(value);
    m_pressed = false;
    QSlider::mouseReleaseEvent(ev);
}

void MySlider::onValueChanged(int value)
{

}
