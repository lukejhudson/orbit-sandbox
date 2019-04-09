#ifndef SIMULATION_H
#define SIMULATION_H

#include <list>
#include <mutex>
#include "body.h"
#include "rocket.h"
#include "sprites.h"

/*
 * Runs the actual simulation. Updates the positions and velocities
 * of each body at regular intervals.
 */
class Simulation {
public:

    enum Mode {
        Background = 0, // Simulation running in the background, non-interactable
        Sandbox = 1,    // Interactable simulation
        Exploration = 2 // No spawning of bodies, focussed on player-controlled rocket
    };

    Simulation(Sprites sprites);
    void resetSim();
    void spawnPlanetarySystem(Body* central, bool spawnRocket);
    void spawnPlanetarySystem(double x, double y, double dx, double dy, bool spawnRocket);
    void spawnPlanetarySystem();
    std::list<Body*>* getBodies();
    void addBody(Body *b);
    [[noreturn]] void run(); // Start the simulation
    void setVisibleRegion(double x, double y, double newWidth, double newHeight, double newScale);
    void setPaused(bool b);
    int getMode();
    void setMode(Mode newMode);
    Rocket* getRocket();
    void setRocket(Rocket *newRocket);
    QRectF calculateValidSpawningRegion();

    QImage* getMap();

private:
    void calculateOrbitVelocity(Body *newBody, Body *central, int maxOrbitDistance);

    std::list<Body*> bodies;
    std::mutex mut; // Mutex used for locking bodies list
    bool paused = true; // Should the sim be paused?
    Sprites sprites;
    double scale = 1; // Matches SimulationWidget's scale
    // Area of visible region
    QRect *visibleRegion;

    Mode mode = Sandbox;
    Rocket *rocket;

    // An image of where the use has explored in Exploration mode
    QImage *exploredMap = nullptr;
    QRect mapDimensions;
    QPoint mapOffset;
};

#endif // SIMULATION_H
