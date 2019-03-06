#ifndef SIMULATIONWINDOW_H
#define SIMULATIONWINDOW_H

#include <QtGui>
#include "rasterwindow.h"
#include "simulation.h"

class SimulationWindow : public RasterWindow {
public:
    SimulationWindow(Simulation *sim, Sprites sprites);
    void setSpawnType(int type);

protected:
    void timerEvent(QTimerEvent *) override;
    void render(QPainter *p) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    int m_timerId;
    std::mutex mut;
    Simulation *sim;
    Sprites sprites;
    Body *newBody;
    bool spawning;
    int spawnType = 0; // Initially asteroid
    Vector initialMousePos;
    Vector mousePos;
    double scale; // Draw objects smaller or larger based on zoom
    int currentOffsetX; // Current offset. Initially 0 for origin top left
    int currentOffsetY;
    int newOffsetX; // Offset of 0 means origin is top left
    int newOffsetY;
    bool movingCamera;
};

#endif // SIMULATIONWINDOW_H
