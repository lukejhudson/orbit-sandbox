#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QMainWindow>
#include <QtWidgets>
#include "simulation.h"
#include "sprites.h"
#include "simulationwindow.h"

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    MainWindow(Simulation *sim, Sprites sprites, SimulationWindow *simwin, QWidget *container);

private:
    SimulationWindow* simwin;
    QPushButton *asteroidButton;
    QPushButton *starButton;
    QPushButton *whitedwarfButton;
    QPushButton *blackholeButton;

private slots:
    void setSpawnType(int type);
};

#endif // MAINWINDOW_H
