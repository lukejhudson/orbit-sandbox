#ifndef ANALOGCLOCK_H
#define ANALOGCLOCK_H

#include <QtGui>

#include "rasterwindow.h"

//! [5]
class AnalogClockWindow : public RasterWindow {
public:
    AnalogClockWindow();

protected:
    void timerEvent(QTimerEvent *) override;
    void render(QPainter *p) override;

private:
    int m_timerId;
};

#endif // ANALOGCLOCK_H
