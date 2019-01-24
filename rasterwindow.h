#ifndef RASTERWINDOW_H
#define RASTERWINDOW_H

#include <QtGui>

class RasterWindow : public QWindow {
    Q_OBJECT // Macro for Qt background stuff
public:
    /*
     * Constructor which allows the window to be a sub-window of another QWindow.
     * Parent-less QWindows show up in the windowing system as top-level windows.
     */
    explicit RasterWindow(QWindow *parent = nullptr);

    virtual void render(QPainter *painter);

public slots: // Slots is a Qt thing, only compiles after sending the code through Qt's preprocessor
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;
    void exposeEvent(QExposeEvent *event) override;

private:
    /*
     * What we use to manage the window's back buffer for QPainter based graphics.
     */
    QBackingStore *m_backingStore;
};

#endif // RASTERWINDOW_H
