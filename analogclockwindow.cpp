/*
 * Example Qt application for reference.
 */

#include "analogclockwindow.h"

AnalogClockWindow::AnalogClockWindow() {
    setTitle("Analog Clock");
    resize(200, 200);
    // Start a timer to be used to redraw the clock every second
    m_timerId = startTimer(1000);
}

/*
 * Called every second as a result of the startTimer call.
 */
void AnalogClockWindow::timerEvent(QTimerEvent *event) {
    if (event->timerId() == m_timerId)
        renderLater(); // Render roughly every second
}

void AnalogClockWindow::render(QPainter *p) {
    // Points of triangular hour hand
    static const QPoint hourHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -40)
    };
    // Points of triangular minute hand
    static const QPoint minuteHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -70)
    };
    // Colour of hands
    QColor hourColor(127, 0, 127);
    QColor minuteColor(0, 127, 127, 191); // 191 --> 75% opaque

    p->setRenderHint(QPainter::Antialiasing);
    // Moves the origin to the centre of the window
    p->translate(width() / 2, height() / 2);
    // Scales the drawing operations so they fit within the window
    int side = qMin(width(), height());
    p->scale(side / 200.0, side / 200.0);

    p->setPen(Qt::NoPen); // No outline
    p->setBrush(hourColor); // Brush = fill

    QTime time = QTime::currentTime();
    /*
     * Save and restore the transformation matrix before and after the rotation
     * because we want to place the minute hand without having to take into
     * account any previous rotations.
     */
    p->save();
    // Rotate to correct angle
    p->rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
    // Draw triangular hand
    p->drawConvexPolygon(hourHand, 3);
    p->restore();

    p->setPen(hourColor);
    // Draw markers around the edge of the clock for each hour
    for (int i = 0; i < 12; ++i) {
        p->drawLine(88, 0, 96, 0);
        p->rotate(30.0);
    }
    p->setPen(Qt::NoPen);
    p->setBrush(minuteColor);

    // Minute hand drawn same as hour hand
    p->save();
    p->rotate(6.0 * (time.minute() + time.second() / 60.0));
    p->drawConvexPolygon(minuteHand, 3);
    p->restore();

    p->setPen(minuteColor);
    // Draw minute markers
    for (int j = 0; j < 60; ++j) {
        if ((j % 5) != 0) // Skip drawing over hour markers
            p->drawLine(92, 0, 96, 0);
        p->rotate(6.0);
    }
}
