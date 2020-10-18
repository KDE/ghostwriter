#include "QtAwesomeAnim.h"

#include <cmath>
#include <QPainter>
#include <QRect>
#include <QTimer>
#include <QWidget>


QtAwesomeAnimation::QtAwesomeAnimation(QWidget *parentWidget, int interval, int step)
    : parentWidgetRef_( parentWidget )
    , timer_( nullptr )
    , interval_( interval )
    , step_( step )
    , angle_( 0.0f )
{

}

void QtAwesomeAnimation::setup( QPainter &painter, const QRect &rect)
{
    // first time set the timer
    if( !timer_ )
    {
        timer_ = new QTimer();
        connect(timer_,SIGNAL(timeout()), this, SLOT(update()) );
        timer_->start(interval_);
    }
    else
    {
        //timer, angle, self.step = self.info[self.parent_widget]
        float x_center = rect.width() * 0.5f;
        float y_center = rect.height() * 0.5f;
        painter.translate(static_cast<qreal>(x_center), static_cast<qreal>(y_center));
        painter.rotate(static_cast<qreal>(angle_));
        painter.translate(static_cast<qreal>(-x_center), -static_cast<qreal>(y_center));
    }
}


void QtAwesomeAnimation::update()
{
    angle_ += step_;
    angle_ = std::fmod( static_cast<float>(angle_), 360.0f);
    parentWidgetRef_->update();
}
