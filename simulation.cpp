#include <thread>
#include <chrono>
#include <iostream>
#include <math.h>
#include "simulation.h"
#include "simulationwindow.h"

// Gravitational constant - Essentially controls the speed of the simulation
#define G  0.005
// #define G  6.67428E-11

/**
 * @brief Simulation::Simulation Initialises the class, adds a star and two
 * asteroids, then starts the simulation in a new thread.
 */
Simulation::Simulation(Sprites sprites) {
    this->sprites = sprites;
    // Add central star to list of bodies
    // resetSim();
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
    scale = 1;
    spawnPlanetarySystem(width / 2, height / 2, 0, 0);
}

/**
 * @brief Simulation::spawnPlanetarySystem Spawns a planetary system with
 * the centre being the given central body. A random number of planets
 * are spawned in orbit of the given central body, with a random number
 * of asteroids orbiting those planets.
 * @param central The central body of the system
 */
void Simulation::spawnPlanetarySystem(Body* central) {
    std::cout << "Spawning plan sys... " << central->getType();
    double x = central->getX();
    double y = central->getY();
    double dx = central->getVelX();
    double dy = central->getVelY();

    // Generate between 2 and 5 planets around the central body
    int numPlanets = 2 + std::rand() % 4;
    int numAsteroids;
    std::list<Body*> newPlanets;
    std::list<Body*> newAsteroids;
    Body *newPlanet;
    Body *newAsteroid;
    double newPX = 0, newPY = 0; // Planet x and y
    double newAX = 0, newAY = 0; // Asteroid x and y
    // Variables used to calculated starting velocity of planets and asteroids
    double centralDiam = central->getDiameter();
    double centralMass = central->getMass();
    Vector centralPos = central->getPos(), planetPos;
    double planetDiam, planetMass;
    double vel, velX, velY, distX, distY;
    double theta;

    // Create new planets
    for (int i = 0; i < numPlanets; i++) {
        newPlanet = new Body(Body::Planet);
        newPlanet->setSprite(sprites.getPlanetImage(newPlanet->getPlanetType()).scaled(static_cast<int>(scale * newPlanet->getDiameter()),
                                                                                       static_cast<int>(scale * newPlanet->getDiameter())));
        // Max distance should depend on central body?
        // Longer for black hole since its gravitational pull is stronger --> Able to keep in orbit further?
        newPX = x + std::rand() % 500 - 250;
        newPY = y + std::rand() % 500 - 250;
        // Centre of new planet must not overlap with the central body
        // (and we want there to be at least a little bit of space between)
        while (pow(newPX - x, 2) + pow(newPY - y, 2) < pow(centralDiam, 2)) {
            newPX = x + std::rand() % 500 - 250;
            newPY = y + std::rand() % 500 - 250;
        }
        newPlanet->setPos(newPX, newPY);
        planetPos = newPlanet->getPos();
        // Calculate velocity of planet
        // v = sqrt( (G * centralMass) / radius of orbit )
        vel = std::sqrt((G * centralMass) / centralPos.distance(planetPos));
        distX = newPX - centralPos.getX();
        distY = newPY - centralPos.getY();
        // Use trig to find the magnitude of the velocity in the x and y directions
        // theta = tan^-1 (|distY| / |distX|)
        theta = std::tanh(distY / distX);
        // velX = vel * sin(theta)
        velX = vel * std::sin(theta);
        // velY = vel * cos(theta)
        velY = vel * std::cos(theta);
        // Flip one of velX or velY to make them go in the correct direction
        // (but we don't mind whether they're going clockwise or anticlockwise)
        if (std::rand() % 2 == 0) {
            velX = -velX;
        } else {
            velY = -velY;
        }
        //std::cout << "dist: " << centralPos.distance(newPlanet->getPos()) << " (" << distX << ", " << distY << ")" << std::endl;
        //std::cout << "vel: " << vel << " (" << velX << ", " << velY << ")" << std::endl;
        newPlanet->setVel(dx + velX, dy + velY);
        newPlanets.push_back(newPlanet);

        // Generate between 0 and 5 asteroids for this planet
        numAsteroids = std::rand() % 6;
        planetDiam = newPlanet->getDiameter();
        planetMass = newPlanet->getMass();
        for (int j = 0; j < numAsteroids; j++) {
            newAsteroid = new Body(Body::Asteroid);
            newAsteroid->setSprite(sprites.asteroidImage.scaled(static_cast<int>(scale * newAsteroid->getDiameter()),
                                                                static_cast<int>(scale * newAsteroid->getDiameter())));
            newAX = newPX + std::rand() % 40 - 20;
            newAY = newPY + std::rand() % 40 - 20;
            // Centre of new asteroid must not overlap with the planet it orbits
            // (and we want there to be at least a little bit of space between)
            while (pow(newAX - newPX, 2) + pow(newAY - newPX, 2) < pow(planetDiam, 2)) {
                newAX = newPX + std::rand() % 40 - 20;
                newAY = newPY + std::rand() % 40 - 20;
            }
            newAsteroid->setPos(newAX, newAY);
            // Calculate velocity of asteroid orbiting planet
            // v = sqrt( (G * planetMass) / radius of orbit )
            vel = std::sqrt((G * planetMass) / planetPos.distance(newAsteroid->getPos()));
            distX = newAX - planetPos.getX();
            distY = newAY - planetPos.getY();
            // Use trig to find the magnitude of the velocity in the x and y directions
            // theta = tan^-1 (|distY| / |distX|)
            theta = std::tanh(distY / distX);
            // velX = vel * sin(theta)
            velX = vel * std::sin(theta);
            // velY = vel * cos(theta)
            velY = vel * std::cos(theta);
            // Flip one of velX or velY to make them go in the correct direction
            // (but we don't mind whether they're going clockwise or anticlockwise)
            if (std::rand() % 2 == 0) {
                velX = -velX;
            } else {
                velY = -velY;
            }
            //std::cout << "dist: " << centralPos.distance(newPlanet->getPos()) << " (" << distX << ", " << distY << ")" << std::endl;
            //std::cout << "vel: " << vel << " (" << velX << ", " << velY << ")" << std::endl;
            newAsteroid->setVel(newPlanet->getVelX() + velX, newPlanet->getVelY() + velY);
            newAsteroids.push_back(newAsteroid);
        }
    }
    // Add bodies to simulation
    mut.lock();
    bodies.push_back(central);
    for (std::list<Body*>::iterator iter = newPlanets.begin(); iter != newPlanets.end(); ++iter) {
        bodies.push_back(*iter);
    }
    for (std::list<Body*>::iterator iter = newAsteroids.begin(); iter != newAsteroids.end(); ++iter) {
        bodies.push_back(*iter);
    }
    mut.unlock();
    std::cout << " Spawning complete" << std::endl;
}

/**
 * @brief Simulation::spawnPlanetarySystem Spawns a planetary system with
 * the centre at the given (x, y) coordinates and a velocity of (dx, dy).
 * The central celestial body is randomly chosen between a star, dwarf star
 * or black hole. A random number of planets are then spawned in orbit
 * around this body, with a random number of asteroids orbiting those planets.
 * @param x x-coordinate of the centre of the planetary system
 * @param y y-coordinate of the centre of the planetary system
 * @param dx x velocity of the planetary system
 * @param dy y velocity of the planetary system
 */
void Simulation::spawnPlanetarySystem(double x, double y, double dx, double dy) {
    // Central body - Type 2 (star), 3 (dwarf star) or 4 (black hole)
    Body *central = new Body(static_cast<Body::BodyType>(2 + std::rand() % 3));
    central->setPos(x, y);
    central->setVel(dx, dy);

    spawnPlanetarySystem(central);
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
                            //     += (pos2 - po1).scale(G * mass2 / (dist ^ 3))
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
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
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
            case Body::Asteroid:
                (*iter)->setSprite(sprites.asteroidImage.scaled(static_cast<int>(scale * (*iter)->getDiameter()),
                                                                static_cast<int>(scale * (*iter)->getDiameter())));
                break;
            case Body::Planet:
                (*iter)->setSprite(sprites.getPlanetImage((*iter)->getPlanetType()).scaled(static_cast<int>(scale * (*iter)->getDiameter()),
                                                                                           static_cast<int>(scale * (*iter)->getDiameter())));
                break;
            case Body::Star:
                (*iter)->setSprite(sprites.starImage.scaled(static_cast<int>(scale * (*iter)->getDiameter()),
                                                            static_cast<int>(scale * (*iter)->getDiameter())));
                break;
            case Body::WhiteDwarf:
                (*iter)->setSprite(sprites.whitedwarfImage.scaled(static_cast<int>(scale * (*iter)->getDiameter()),
                                                                  static_cast<int>(scale * (*iter)->getDiameter())));
                break;
            case Body::BlackHole:
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

/**
 * @brief Simulation::setWidth Sets the width of the visiable region to w.
 * @param w The width of the visible region
 */
void Simulation::setWidth(double w) {
    width = w;
}

/**
 * @brief Simulation::setHeight Sets the height of the visible region to h.
 * @param h The height of the visible region
 */
void Simulation::setHeight(double h) {
    height = h;
}










