#ifndef SIMULATION_H
#define SIMULATION_H

#include <list>
#include <mutex>
#include "body.h"
#include "sprites.h"

/*
 * Runs the actual simulation. Updates the positions and velocities
 * of each body at regular intervals.
 */
class Simulation {
public:
    Simulation(Sprites sprites);
    void resetSim();
    void spawnPlanetarySystem(Body* central);
    void spawnPlanetarySystem(double x, double y, double dx, double dy);
    std::list<Body*>* getBodies(); // Get list of bodies
    void addBody(Body *b); // Add 1 body to simulation
    void run(); // Start the simulation
    void setSprites(double scale);
    void setPaused(bool b);
    void setWidth(double w);
    void setHeight(double h);

private:
    std::list<Body*> bodies;
    std::mutex mut; // Mutex used for locking bodies list
    bool paused = true; // Should the sim be paused?
    Sprites sprites;
    double scale = 1; // Matches SimulationWindow's scale
    // Width and height of visible region
    double width = 100;
    double height = 100;
};

#endif // SIMULATION_H
