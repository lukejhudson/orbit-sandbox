#ifndef SIMULATIONWINDOW_H
#define SIMULATIONWINDOW_H

#include <QtGui>
#include "simulation.h"

class SimulationWindow : public QWindow {
    Q_OBJECT
public:
    SimulationWindow(Simulation *sim, Sprites sprites, double width, double height);
    void setSpawnType(Body::BodyType type);

    virtual void render(QPainter *painter);

public slots:
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void exposeEvent(QExposeEvent *event) override;
    void timerEvent(QTimerEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    // Used to manage the window's back buffer for QPainter based graphics
    QBackingStore *m_backingStore;

    int m_timerId;
    std::mutex mut;
    Simulation *sim;
    Sprites sprites;
    Body *newBody;
    bool spawning;
    Body::BodyType spawnType = Body::Asteroid; // Initially asteroid
    Vector initialMousePos;
    Vector mousePos;
    double scale; // Draw objects smaller or larger based on zoom
    double currentOffsetX; // Current offset. Initially 0 for origin top left
    double currentOffsetY;
    double newOffsetX; // Offset of 0 means origin is top left
    double newOffsetY;
    bool movingCamera;

};

#endif // SIMULATIONWINDOW_H
