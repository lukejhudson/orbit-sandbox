#ifndef ANALOGCLOCKWINDOW_H
#define ANALOGCLOCKWINDOW_H

#include <QtGui>
#include "rasterwindow.h"

class AnalogClockWindow : public RasterWindow {
public:
    AnalogClockWindow();

protected:
    void timerEvent(QTimerEvent *) override;
    void render(QPainter *p) override;

private:
    int m_timerId;
};

#endif // ANALOGCLOCKWINDOW_H
