#include "rasterwindow.h"

/*
 * Constructor.
 * Create the backingstore and pass it the window instance it is supposed to manage.
 * We also set the initial window geometry.
 */
RasterWindow::RasterWindow(QWindow *parent) : QWindow(parent), m_backingStore(new QBackingStore(this)) {
    // Set size of window: x, y, width, height
    setGeometry(100, 100, 300, 200);
    /*
     * Stuff after the ":" is shorthand for:
     * QWindow = parent;
     * m_backingStore = new QBackingStore(this);
     */
}

/*
 * Handle all the different possible events.
 */
bool RasterWindow::event(QEvent *event) {
    // Handle update event
    if (event->type() == QEvent::UpdateRequest) {
        // Render window
        renderNow();
        return true;
    }
    // Make sure the rest of events are handled
    return QWindow::event(event);
}

/*
 * Don't need to render immediately, so instead request for an update when the
 * system is ready to repaint.
 */
void RasterWindow::renderLater() {
    requestUpdate();
}

/*
 * The resize event is guaranteed to be called prior to the window being shown
 * on screen and will also be called whenever the window is resized while on screen.
 * We use this to resize the back buffer, and defer rendering to the
 * corresponding/following expose event.
 */
void RasterWindow::resizeEvent(QResizeEvent *resizeEvent) {
    m_backingStore->resize(resizeEvent->size());
}

/*
 * Called after the window is set to be shown (window.show(); in main)
 * to notify us that the window's exposure in the windowing system has changed.
 * (Also called when the window becomes obscured)
 */
void RasterWindow::exposeEvent(QExposeEvent *) {
    if (isExposed()) // Is the window showing?
        renderNow(); // Yes --> Draw immediately
}

void RasterWindow::renderNow() {
    if (!isExposed())
        return; // Not showing --> Don't bother rendering

    // Area we want to paint
    QRect rect(0, 0, width(), height());
    // Paint area
    m_backingStore->beginPaint(rect);

    // Get the QPaintDevice of the back buffer
    QPaintDevice *device = m_backingStore->paintDevice();
    // Creae a QPainter to render to the paint device
    QPainter painter(device);

    // Draw the gradient background on the window
    painter.fillRect(0, 0, width(), height(), QGradient::NightFade);
    render(&painter);
    painter.end();

    // Done rendering
    m_backingStore->endPaint();
    // Present the contents in the back buffer
    m_backingStore->flush(rect);

    /*
     * Want to render 60 (?) times each second --> Timer similar to clock
     * Grab list of bodies from simulation and draw them on the screen
     * If LMB down --> Draw extra body (but not part of simulation so no interactions)
     * and line from mouse to body, arrow from body forwards (and potentially expected path later)
     */
}

/*
 * The render function contains the drawing code for the window.
 */
void RasterWindow::render(QPainter *painter) {
    painter->drawText(QRectF(0, 0, width(), height()), Qt::AlignCenter, QStringLiteral("QWindow"));
}
