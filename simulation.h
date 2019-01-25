#ifndef SIMULATION_H
#define SIMULATION_H

#include <list>
#include <mutex>
#include "body.h"

/*
 * Runs the actual simulation. Updates the positions and velocities
 * of each body at regular intervals.
 */
class Simulation {
public:
    Simulation();
    std::list<Body> getBodies(); // Get list of bodies
    void addBody(Body b); // Add 1 body to simulation
    void run(); // Start the simulation

private:
    std::list<Body> bodies;
    std::mutex mut; // Mutex used for locking bodies list
};

#endif // SIMULATION_H
