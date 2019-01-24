#include <thread>
#include <chrono>
#include <iostream>
#include <math.h>
#include "simulation.h"

// Gravitational constant
#define G  0.3
// #define G  6.67428E-11

/**
 * @brief Simulation::Simulation Initialises the class, adds a star and two
 * asteroids, then starts the simulation in a new thread.
 */
Simulation::Simulation() {
    width = 500;
    height = 500;
    // Add central star to list of bodies
    bodies.push_front(*new Body(1000, 50, new Vector(width / 2, height / 2), new Vector(0, 0), 0));
    bodies.push_back(*new Body(5, 5, new Vector(200, 200), new Vector(1, 0), 1));
    bodies.push_back(*new Body(5, 5, new Vector(400, 400), new Vector(-2, 0), 1));
    std::cout << "Starting simulation... ";
    std::thread t(&Simulation::run, this);
    t.detach();
    std::cout << "Simulation started" << std::endl;
}

/**
 * @brief Simulation::getBodies Returns the list of bodies which are active
 * in the simulation.
 * @return The list of bodies in the simulation
 */
std::list<Body> Simulation::getBodies() {
    /*
     * Create new list
     * Lock
     * Loop
     *      Create new Body
     *      Copy values from body in bodies
     *      Add new body to list
     * Unlock
     * Return new list
     */
    return bodies;
}

/**
 * @brief Simulation::addBody Adds the given body to the simulation.
 * @param b The body to add to the simulation
 */
void Simulation::addBody(Body b) {
    mut.lock();
    bodies.push_back(b);
    mut.unlock();
}

/**
 * @brief Simulation::run Runs the simulation by calculating the gravitational forces
 * acting on all bodies, then adjusts their velocity accordingly. Updates the position
 * of all bodies based on their velocity.
 */
void Simulation::run() {
    while (true) {
        //std::cout << "Calcualtions" << std::endl;
        // For each body
        for (std::list<Body>::iterator iter1 = bodies.begin(); iter1 != bodies.end(); ++iter1) {
            // For each other body
            for (std::list<Body>::iterator iter2 = bodies.begin(); iter2 != bodies.end(); ++iter2) {
                // Make sure the two bodies are not the same
                if (iter1 != iter2) {
                    // Calculate gravitational force exerted on body and update velocity
                    // vel += (G * mass2 * (pos2 - pos 1)) / (dist ^ 3)
                    // vel += (pos2 - po1).scale(G * mass2 / (dist ^ 3))
                    //std::cout << "vel before: " << iter1->getVel().toString();
                    iter1->setVel(
                        iter1->getVel().add(
                            (iter2->getPos().sub(iter1->getPos())).scale(
                                G * iter2->getMass() / pow(iter2->getPos().distance(iter1->getPos()), 3))
                        )
                    );
                    //std::cout << "\tvel after: " << iter1->getVel().toString() << std::endl;
                }
            }
        }
        //std::cout << "Moving" << std::endl;
        // For each body
        for (std::list<Body>::iterator iter = bodies.begin(); iter != bodies.end(); ++iter) {
            //std::cout << "Moving body: " << iter->toString() << std::endl;
            // Update position
            iter->move();
            // Hit star --> (lock) delete / explode? (unlock)
        }
        //std::cout << "Done moving" << std::endl;
        // Sleep 10 ms
        //std::cout << "Sleeping" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(17));
        //std::cout << "Done sleeping" << std::endl;
    }
}


















