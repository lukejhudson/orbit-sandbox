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
    MainWindow();

private:
    Sprites *sprites;
    // Simulation itself
    Simulation *sim;
    // Controls simulation
    SimulationWindow *simwin;

    // Create all components for the main menu
    void createMainMenu();
    // Remove all components for the main menu
    void clearMainMenu();
    // Elements for main menu
    QWidget *mmContainer;
    QVBoxLayout *mmvLayout;
    QLabel *title;
    QLabel *credits;
    QPushButton *sandboxModeButton;
    QPushButton *controlsScreenButton;

    // Create all components for sandbox mode
    void createSandboxMode();
    // Remove all components for sandbox mode
    void clearSandboxMode();
    // Cycle the planet icons between the 5 possible sprites
    void cyclePlanetIcons();
    // Should the icons be cycled?
    bool iconCycleRunning;
    // Elements for sandbox mode
    QWidget *sbContainer;
    QHBoxLayout *sbhLayout;
    QVBoxLayout *sbvLayout;
    QLabel *spawnLabel;
    QWidget *asteroidContainer;
    QVBoxLayout *vAsteroidLayout;
    QPushButton *asteroidButton;
    QLabel *asteroidLabel;
    QWidget *planetContainer;
    QVBoxLayout *vPlanetLayout;
    QPushButton *planetButton;
    QLabel *planetLabel;
    QWidget *starContainer;
    QVBoxLayout *vStarLayout;
    QPushButton *starButton;
    QLabel *starLabel;
    QWidget *whitedwarfContainer;
    QVBoxLayout *vWhitedwarfLayout;
    QPushButton *whitedwarfButton;
    QLabel *whitedwarfLabel;
    QWidget *blackholeContainer;
    QVBoxLayout *vBlackholeLayout;
    QPushButton *blackholeButton;
    QLabel *blackholeLabel;
    QWidget *plansysContainer;
    QVBoxLayout *vPlansysLayout;
    QPushButton *plansysButton;
    QLabel *plansysLabel;
    QPushButton *sbHomeButton;

    // Create all components for the controls screen
    void createControlsScreen();
    // Remove all components for the controls screen
    void clearControlsScreen();
    // Elements for main menu
    QWidget *csContainer;
    QHBoxLayout *cshLayout;
    QVBoxLayout *csvLayout;
    QLabel *csTitle;
    QWidget *csSandboxContainer;
    QVBoxLayout *csvSandboxLayout;
    QLabel *csSandboxTitle;
    QLabel *csSandboxText;
    QPushButton *csHomeButton;

private slots:
    void setSpawnType(Body::BodyType type);
};

#endif // MAINWINDOW_H
