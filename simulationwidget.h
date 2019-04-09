#ifndef SIMULATIONWIDGET_H
#define SIMULATIONWIDGET_H

#include <QtWidgets>
#include "simulation.h"

class SimulationWidget : public QWidget {
    Q_OBJECT
public:
    SimulationWidget(Simulation *sim, Sprites sprites);
    void setSpawnType(Body::BodyType type);

signals:
    void gameOverSignal();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    Simulation *sim;
    Sprites sprites;
    Body *newBody;
    bool spawning = false;
    Body::BodyType spawnType = Body::Asteroid; // Initially asteroid
    QPointF *initialMousePos;
    QPointF *mousePos;
    // Draw objects smaller or larger based on zoom (zoomed in = larger scale = larger sprites)
    double scale = 1;
    // Current offset. Initially (0, 0) for origin top left
    QPointF *currentOffset;
    // Offset for the background
    QPointF *backgroundOffset;
    // Offset of (0, 0) means origin is top left
    QPointF *newOffset;
    // How many background images we would have to travel back over to get to the origin
    QPointF *totalBackgrounds;
    bool movingCamera = false;

    // Triggered when the player dies and the explosion animation finished in exploration mode
    bool gameOver = false;

    void updateSimVisibleRegion();
};

#endif // SIMULATIONWIDGET_H
