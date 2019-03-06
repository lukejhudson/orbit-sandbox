#include <thread>
#include <chrono>
#include <iostream>
#include <math.h>
#include "simulation.h"
#include "simulationwindow.h"

// Gravitational constant
#define G  0.3
// #define G  6.67428E-11

/**
 * @brief Simulation::Simulation Initialises the class, adds a star and two
 * asteroids, then starts the simulation in a new thread.
 */
Simulation::Simulation(Sprites sprites) {
    this->sprites = sprites;
    // Add central star to list of bodies
    resetSim();
    std::cout << "Starting simulation... ";
    std::thread t(&Simulation::run, this);
    t.detach();
    std::cout << "Simulation started" << std::endl;
}

/**
 * @brief Simulation::resetSim Resets the simulation to default settings.
 * Removes all bodies and adds in the inital three.
 */
void Simulation::resetSim() {
    // Delete current bodies list
    mut.lock();
    for (std::list<Body*>::iterator iter = bodies.begin(); iter != bodies.end(); ++iter) {
        delete *iter;
    }
    mut.unlock();
    bodies.erase(bodies.begin(), bodies.end());
    // Add default bodies
    bodies.push_front(new Body(1000, 50, new Vector(250, 250), new Vector(0, 0), 2, sprites.starImage));
    bodies.push_back(new Body(5, 5, new Vector(200, 200), new Vector(1, 0), 0, sprites.asteroidImage));
    bodies.push_back(new Body(5, 5, new Vector(400, 400), new Vector(-2, 0), 0, sprites.asteroidImage));
}

/**
 * @brief Simulation::getBodies Returns a copy of list of bodies which are active
 * in the simulation.
 * @return A copy of the list of bodies in the simulation (must be deleted)
 */
std::list<Body*>* Simulation::getBodies() {
    std::list<Body*> *bodiesCopy = new std::list<Body*>();
    mut.lock();
    Body *b;
    for (std::list<Body*>::iterator iter = bodies.begin(); iter != bodies.end(); ++iter) {
        b = (*iter)->copy();
        bodiesCopy->push_back(b);
    }
    mut.unlock();
    return bodiesCopy;
}

/**
 * @brief Simulation::addBody Adds the given body to the simulation.
 * @param b The body to add to the simulation
 */
void Simulation::addBody(Body *b) {
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
        if (!paused) {
            // std::cout << "Calculations" << std::endl;
            // For each body
            for (std::list<Body*>::iterator iter1 = bodies.begin(); iter1 != bodies.end(); ++iter1) {
                // For each other body
                for (std::list<Body*>::iterator iter2 = bodies.begin(); iter2 != bodies.end(); ++iter2) {
                    // Make sure the two bodies are not the same and that they are both active
                    if (iter1 != iter2 && (*iter1)->isActive() && (*iter2)->isActive()) {
                        // If collision (dist between is less than half the sum of their diameters)
                        if ((*iter1)->getPos().squareDist((*iter2)->getPos()) <
                                pow((*iter1)->getDiameter() / 2, 2) + pow((*iter2)->getDiameter() / 2, 2)) {
                            //std::cout << "COLLISION" << std::endl;
                            // Combine the two colliding bodies, and mark the smaller body for removal
                            if ((*iter1)->getMass() >= (*iter2)->getMass()) {
                                (*iter1)->combine(*iter2);
                                (*iter2)->setActive(false);
                                (*iter1)->resizeSprite(sprites, scale);
                            } else {
                                (*iter2)->combine(*iter1);
                                (*iter1)->setActive(false);
                                (*iter2)->resizeSprite(sprites, scale);
                            }
                        } else {
                            // Calculate gravitational force exerted on body and update velocity
                            // vel += (G * mass2 * (pos2 - pos 1)) / (dist ^ 3)
                            // vel += (pos2 - po1).scale(G * mass2 / (dist ^ 3))
                            //std::cout << "vel before: " << (*iter1)->getVel().toString();
                            double dist = (*iter2)->getPos().distance((*iter1)->getPos());
                            if (dist < 1) dist = 1;
                            Vector body2;
                            body2.set((*iter2)->getPos());
                            (*iter1)->setVel( // Set velocity
                                (*iter1)->getVel().add( // Add following to current velocity
                                    (body2.sub((*iter1)->getPos())).scale( // (pos2 - pos1) scaled by
                                        G * (*iter2)->getMass() / // G * mass2 divided by
                                            pow(dist, 3)) // (dist^3)
                                )
                            );
                            //std::cout << "\tvel after: " << (*iter1)->getVel().toString() << std::endl;
                        }
                    }
                }
            }
            for (std::list<Body*>::iterator iter = bodies.begin(); iter != bodies.end(); ++iter) {
                //std::cout << "Moving body: " << iter->toString() << std::endl;
                // Update position if the body is active
                if ((*iter)->isActive()) {
                    (*iter)->move();
                } else {
                    // Remove body if it isn't active
                    mut.lock();
                    delete *iter;
                    iter = bodies.erase(iter);
                    mut.unlock();
                }
            }
        }
        // Sleep to maintain ~60 ticks per second
        std::this_thread::sleep_for(std::chrono::milliseconds(17));
    }
}

/**
 * @brief Simulation::setSprites Sets the sprites of all bodies and scales them to the correct size.
 */
void Simulation::setSprites(double s) {
    // Takes the full sized sprite for the body's type and scales it to the correct size
    scale = s;
    for (std::list<Body*>::iterator iter = bodies.begin(); iter != bodies.end(); ++iter) {
        switch ((*iter)->getType()) {
            case 0: // Asteroid
                (*iter)->setSprite(sprites.asteroidImage.scaled(static_cast<int>(scale * (*iter)->getDiameter()),
                                                                static_cast<int>(scale * (*iter)->getDiameter())));
                break;
            case 1: // Planet
                (*iter)->setSprite(sprites.getPlanetImage((*iter)->getPlanetType()).scaled(static_cast<int>(scale * (*iter)->getDiameter()),
                                                                                           static_cast<int>(scale * (*iter)->getDiameter())));
                break;
            case 2: // Star
                (*iter)->setSprite(sprites.starImage.scaled(static_cast<int>(scale * (*iter)->getDiameter()),
                                                            static_cast<int>(scale * (*iter)->getDiameter())));
                break;
            case 3: // White dwarf
                (*iter)->setSprite(sprites.whitedwarfImage.scaled(static_cast<int>(scale * (*iter)->getDiameter()),
                                                                  static_cast<int>(scale * (*iter)->getDiameter())));
                break;
            case 4: // Black hole
                (*iter)->setSprite(sprites.blackholeImage.scaled(static_cast<int>(scale * (*iter)->getDiameter()),
                                                                 static_cast<int>(scale * (*iter)->getDiameter())));
                break;
        }
    }
}

/**
 * @brief Simulation::setPaused Sets whether the simulation should be paused.
 * @param b True if the simulation should be paused
 */
void Simulation::setPaused(bool b) {
    paused = b;
}












