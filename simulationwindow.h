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

private:
    int m_timerId;
    std::mutex mut;
    Simulation *sim;
    Body newBody;
    bool spawning;
    Vector mousePos;
};

#endif // SIMULATIONWINDOW_H
