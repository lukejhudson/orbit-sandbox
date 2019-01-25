#ifndef SIMULATIONWINDOW_H
#define SIMULATIONWINDOW_H

#include <QtGui>
#include "rasterwindow.h"
#include "simulation.h"

class SimulationWindow : public RasterWindow {
public:
    SimulationWindow(Simulation *sim);

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
    Body newBody;
    bool spawning;
    Vector mousePos;
    double scale; // Draw objects smaller or larger based on zoom
    int viewOffsetX; // Offset of 0 means origin is top left
    int viewOffsetY;
};

#endif // SIMULATIONWINDOW_H
