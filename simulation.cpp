#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>
#include "simulation.h"
#include "simulationwidget.h"

#define MAX_SYSTEM_ORBIT_RADIUS 250
#define MAX_PLANET_ORBIT_RADIUS 20
// How scaled down the map is compared to the user's view
#define MAP_SCALE 100.0

/**
 * @brief Simulation::Simulation Initialises the class, adds a star and two
 * asteroids, then starts the simulation in a new thread.
 */
Simulation::Simulation(Sprites sprites) {
    this->sprites = sprites;
    visibleRegion = new QRect(0, 0, 100, 100);
    // Add central star to list of bodies
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
    scale = 1;
    // Spawn initial planetary system in the centre of the screen, along
    // with a player-controlled rocket if we are in the Exploration mode
    spawnPlanetarySystem(0, 0, 0, 0, mode == Exploration);

    // Create new, empty map
    if (exploredMap) delete exploredMap;
    exploredMap = new QImage(1000, 1000, QImage::Format_ARGB32_Premultiplied);
    exploredMap->fill(QColor(255,255,255));
    mapDimensions.setRect(-500, -500, 1000, 1000);
    mapOffset.setX(0);
    mapOffset.setY(0);
}

/**
 * @brief Simulation::calculateOrbitVelocity Spawns the given newBody at a
 * random distance away from central, according to maxOrbitDistance, then
 * calculates the velocity required to stay in orbit. The calculate position
 * and velocity Vectors are placed into the given newBody body.
 * @param newBody The body to place around central and calculate its orbit
 * velocity
 * @param central The body which newBody orbits
 * @param maxOrbitDistance The maximum distance from central that newBody
 * should be placed at
 */
void Simulation::calculateOrbitVelocity(Body *newBody, Body *central, int maxOrbitDistance) {
    double x = central->getX();
    double y = central->getY();
    double centralDiam = central->getDiameter();
    double centralMass = central->getMass();
    Vector centralPos = central->getPos();
    // Max distance should depend on central body?
    // Longer for black hole since its gravitational pull is stronger --> Able to keep in orbit further?
    double newX = x + rand() % (2 * maxOrbitDistance) - maxOrbitDistance;
    double newY = y + rand() % (2 * maxOrbitDistance) - maxOrbitDistance;
    // Centre of newBody must not overlap with the central body
    // (and we want there to be at least a little bit of space between)
    while (pow(newX - x, 2) + pow(newY - y, 2) < pow(centralDiam, 2)) {
        newX = x + rand() % (2 * maxOrbitDistance) - maxOrbitDistance;
        newY = y + rand() % (2 * maxOrbitDistance) - maxOrbitDistance;
    }
    newBody->setPos(newX, newY);
    Vector newBodyPos = newBody->getPos();
    // Calculate velocity of newBody
    // v = sqrt( (G * centralMass) / radius of orbit )
    double vel = sqrt((G * centralMass) / centralPos.distance(newBodyPos));
    double distX = newX - centralPos.getX();
    double distY = newY - centralPos.getY();
    // Use trig to find the magnitude of the velocity in the x and y directions
    // theta = tan^-1 (|distY| / |distX|)
    double theta = tanh(distY / distX);
    // velX = vel * sin(theta)
    double velX = vel * sin(theta);
    // velY = vel * cos(theta)
    double velY = vel * cos(theta);
    // Flip one of velX or velY to make the newBody go in the correct direction
    // (but we don't mind whether they're going clockwise or anticlockwise)
    if (rand() % 2 == 0) {
        velX = -velX;
    } else {
        velY = -velY;
    }
    newBody->setVel(central->getVelX() + velX, central->getVelY() + velY);
}

/**
 * @brief Simulation::spawnPlanetarySystem Spawns a planetary system with
 * the centre being the given central body. A random number of planets
 * are spawned in orbit of the given central body, with a random number
 * of asteroids orbiting those planets.
 * @param central The central body of the system
 * @param spawnRocket Should a player-controlled rocket be spawned with
 * this planetary system? This should only be true if this is the first
 * planetary system to be spawned
 */
void Simulation::spawnPlanetarySystem(Body* central, bool spawnRocket) {
    std::list<Body*> newPlanets;
    std::list<Body*> newAsteroids;
    Body *newPlanet;
    Body *newAsteroid;
    int numAsteroids;

    // Generate between 2 and 5 planets around the central body
    int numPlanets = 2 + std::rand() % 4;
    for (int i = 0; i < numPlanets; i++) {
        newPlanet = new Body(Body::Planet);
        calculateOrbitVelocity(newPlanet, central, MAX_SYSTEM_ORBIT_RADIUS);
        newPlanets.push_back(newPlanet);

        if (spawnRocket && i == 0 && mode == Exploration) {
            // Spawn Rocket rather than asteroids
            rocket = new Rocket();
            calculateOrbitVelocity(rocket, newPlanet, MAX_PLANET_ORBIT_RADIUS);
        } else {
            // Generate between 0 and 5 asteroids for this planet
            numAsteroids = rand() % 6;
            for (int j = 0; j < numAsteroids; j++) {
                newAsteroid = new Body(Body::Asteroid);
                calculateOrbitVelocity(newAsteroid, newPlanet, MAX_PLANET_ORBIT_RADIUS);
                newAsteroids.push_back(newAsteroid);
            }
        }
    }
    // Add bodies to simulation
    mut.lock();
    if (spawnRocket && mode == Exploration) {
        bodies.push_back(rocket);
    }
    bodies.push_back(central);
    for (std::list<Body*>::iterator iter = newPlanets.begin(), end = newPlanets.end(); iter != end; ++iter) {
        bodies.push_back(*iter);
    }
    for (std::list<Body*>::iterator iter = newAsteroids.begin(), end = newAsteroids.end(); iter != end; ++iter) {
        bodies.push_back(*iter);
    }
    mut.unlock();
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
 * @param spawnRocket Should a player-controlled rocket be spawned with
 * this planetary system? This should only be true if this is the first
 * planetary system to be spawned
 */
void Simulation::spawnPlanetarySystem(double x, double y, double dx, double dy, bool spawnRocket) {
    // Central body - Type 2 (star), 3 (dwarf star) or 4 (black hole)
    Body *central = new Body(static_cast<Body::BodyType>(2 + rand() % 3));
    central->setPos(x, y);
    central->setVel(dx, dy);

    spawnPlanetarySystem(central, spawnRocket);
}

/**
 * @brief Simulation::spawnPlanetarySystem Handles the procedural generation
 * of planetary systems. Decides whether one or more new systems should be
 * spawned and randomly generates their velocities and a valid starting position.
 * The new systems are then generated and added to the simulation.
 */
void Simulation::spawnPlanetarySystem() {
    QRectF validSpawningRegion = calculateValidSpawningRegion();
    double newX, newY;
    Vector *pos;
    Body *central;

    // 10 attempts when zoomed all the way in, 110 all the way out
    int numAttempts = static_cast<int>((validSpawningRegion.width() *
            validSpawningRegion.height()) / 400000.0);

    for (int i = 0; i < numAttempts; i++) {
        // New central body of the system
        // Type 2 (star), 3 (dwarf star) or 4 (black hole)
        // Calculate random pos, anywhere in the spawning region
        newX = validSpawningRegion.x() + rand() % static_cast<int>(validSpawningRegion.width());
        newY = validSpawningRegion.y() + rand() % static_cast<int>(validSpawningRegion.height());
        pos = new Vector(newX, newY);

        // Calculate position on exploredMap and find if that pixel is blue (has been explored)
        if (static_cast<QRgb>(
                    exploredMap->pixel(
                        static_cast<int>((mapOffset.x() + mapDimensions.x() +
                                          mapDimensions.width()) + pos->getX() / MAP_SCALE),
                        static_cast<int>((mapOffset.y() + mapDimensions.y() +
                                          mapDimensions.height()) + pos->getY() / MAP_SCALE)))
                == QRgb(qRgb(0,0,255))) {
            // Area which we are trying to spawn at is invalid - it has been explored already
            // -> Failed attempt, break
            break;
        }

        // Invalid if:
            // Any part of the new system is visible as it spawns
            // The system overlaps with another system
        bool valid = true;

        // Make sure the new system is not initially visible
        // --> Don't want it to pop in suddenly
        if (newX >= rocket->getX() - visibleRegion->width() / 2 - MAX_SYSTEM_ORBIT_RADIUS
                && newX <= rocket->getX() + visibleRegion->width() / 2 + MAX_SYSTEM_ORBIT_RADIUS
                && newY >= rocket->getY() - visibleRegion->height() / 2 - MAX_SYSTEM_ORBIT_RADIUS
                && newY <= rocket->getY() + visibleRegion->height() / 2 + MAX_SYSTEM_ORBIT_RADIUS) {
            valid = false;
        } else {
            int type;
            for (std::list<Body*>::iterator iter = bodies.begin(), end = bodies.end(); iter != end; ++iter) {
                type = (*iter)->getType();
                // Make sure the new body doesn't overlap with any other system
                if ((type == Body::Star || type == Body::WhiteDwarf || type == Body::BlackHole)
                        && (*iter)->getPos().squareDist(pos) < pow(MAX_SYSTEM_ORBIT_RADIUS, 2) * 2) {
                    valid = false;
                }
            }
        }

        if (valid) {
            // Valid --> Spawn
            central = new Body(static_cast<Body::BodyType>(2 + rand() % 3));
            central->setPos(newX, newY);
            // Velocity between -0.5 and 0.5 in both x and y
            central->setVel(-0.5 + (rand() % 100) / 100.0,
                            -0.5 + (rand() % 100) / 100.0);
            // Spawn the system around the central body
            spawnPlanetarySystem(central, false);
        }
    }
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
    for (std::list<Body*>::iterator iter = bodies.begin(), end = bodies.end(); iter != end; ++iter) {
        if ((*iter)->getType() == Body::PlayerRocket) {
            b = static_cast<Rocket*>((*iter))->copy();
        } else {
            b = (*iter)->copy();
        }
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
    // Keep track of how long the current loop has taken
    // --> Try to get closer to 60 ticks / sec
    std::chrono::_V2::system_clock::time_point tickStartTime;
    std::chrono::_V2::system_clock::time_point tickEndTime;
    std::chrono::duration<double, std::milli> tickElapsedTime;
    int loopCount = 0;

    // Create some frequently used variables
    Vector iter1Pos, iter2Pos;
    double iter1X, iter1Y, iter1Diam, iter2X, iter2Y, iter2Diam;
    bool collision = false;

    while (true) {
        if (!paused) {
            tickStartTime = std::chrono::high_resolution_clock::now();
            if (mode == Exploration) {
                if (loopCount % 30 == 0) {
                    spawnPlanetarySystem();

                    // Update map with area explored
                    QRectF exploredRegion = *visibleRegion;//calculateValidSpawningRegion();
                    double x = exploredRegion.x() / MAP_SCALE, y = exploredRegion.y() / MAP_SCALE;
                    double w = exploredRegion.width() / MAP_SCALE, h = exploredRegion.height() / MAP_SCALE;
                    // Resize map if necessary
                    bool resize = false;
                    QRect rect;
                    if (x < mapDimensions.x()) {
                        rect = mapDimensions;
                        mapDimensions.setX(mapDimensions.x() - 1000);
                        resize = true;
                        mapOffset.setX(mapOffset.x() + 1000);
                    } else if (x + w > mapDimensions.x() + mapDimensions.width()) {
                        rect = mapDimensions;
                        mapDimensions.setWidth(mapDimensions.width() + 1000);
                        resize = true;
                        mapOffset.setX(mapOffset.x() - 1000);
                    } else if (y < mapDimensions.y()) {
                        rect = mapDimensions;
                        mapDimensions.setY(mapDimensions.y() - 1000);
                        resize = true;
                        mapOffset.setY(mapOffset.y() + 1000);
                    } else if (y + h > mapDimensions.y() + mapDimensions.height()) {
                        rect = mapDimensions;
                        mapDimensions.setHeight(mapDimensions.height() + 1000);
                        resize = true;
                        mapOffset.setY(mapOffset.y() - 1000);
                    }
                    if (resize) {
                        QImage *newMap = new QImage(mapDimensions.size(), QImage::Format_ARGB32_Premultiplied);
                        newMap->fill(QColor(255, 255, 255));
                        QPainter p(newMap);
                        p.drawImage(rect.topLeft() - mapDimensions.topLeft(), *exploredMap);
                        QImage *pointer = exploredMap;
                        exploredMap = newMap;
                        delete pointer;
                    }
                    // Fill out map with area explored
                    QPainter p(exploredMap);
                    p.fillRect(static_cast<int>((mapOffset.x() + mapDimensions.x() + mapDimensions.width()) + x),
                               static_cast<int>((mapOffset.y() + mapDimensions.y() + mapDimensions.height()) + y),
                               static_cast<int>(w),
                               static_cast<int>(h),
                               QColor(0, 0, 255));
                }
            }

            // For each body
            for (std::list<Body*>::iterator iter1 = bodies.begin(), end1 = bodies.end(); iter1 != end1; ++iter1) {
                // For each other body
                for (std::list<Body*>::iterator iter2 = bodies.begin(), end2 = bodies.end(); iter2 != end2; ++iter2) {
                    // Make sure the two bodies are not the same and that they are both active
                    if (iter1 != iter2 && (*iter1)->isActive() && (*iter2)->isActive()) {
                        // Initialise frequently used variables
                        iter1Pos = (*iter1)->getPos();
                        iter1X = iter1Pos.getX();
                        iter1Y = iter1Pos.getY();
                        iter1Diam = (*iter1)->getDiameter();
                        iter2Pos = (*iter2)->getPos();
                        iter2X = iter2Pos.getX();
                        iter2Y = iter2Pos.getY();
                        iter2Diam = (*iter2)->getDiameter();

                        // If collision (dist between is less than half the sum of their diameters)
                        //if ((*iter1)->getPos().squareDist((*iter2)->getPos()) <
                        //        pow((*iter1)->getDiameter() / 2, 2) + pow((*iter2)->getDiameter() / 2, 2)) {

                        collision = false;
                        // Are the bodies even remotely close to each other?
                        if (fabs(iter1X - iter2X) + fabs(iter1Y - iter2Y) < 500) {
                            // Assuming the two bodies are rectangles, do they overlap?
                            if (fabs(iter1X - iter2X) < ((iter1Diam + iter2Diam) / 2)
                                    && fabs(iter1Y - iter2Y) < ((iter1Diam + iter2Diam) / 2)) {
                                // The rectangles overlap --> Is either body on screen?
                                // Zoomed out too far     --> Accurate collision detection not important, good enough
                                // Both off screen        --> Good enough
                                // One or both on screen  --> Further checks
                                if (scale > 0.3
                                      && ((*iter1)->isWithin(*visibleRegion)
                                      || (*iter2)->isWithin(*visibleRegion))) {
                                    // On screen --> Check if sprites overlap

                                    // Get the sprites of the bodies
                                    QPixmap sprite1, sprite2;
                                    if ((*iter1)->getType() == Body::PlayerRocket) {
                                        sprite1 = sprites.rocketIdleImage;
                                        sprite2 = sprites.getImage(*iter2);
                                    } else if ((*iter2)->getType() == Body::PlayerRocket) {
                                        sprite1 = sprites.getImage(*iter1);
                                        sprite2 = sprites.rocketIdleImage;
                                    } else {
                                        sprite1 = sprites.getImage(*iter1);
                                        sprite2 = sprites.getImage(*iter2);
                                    }
                                    // Resize the sprites so they're as big as they are on screen
                                    sprite1 = sprite1.scaled(static_cast<int>(iter1Diam * scale),
                                                             static_cast<int>(iter1Diam * scale));
                                    sprite2 = sprite2.scaled(static_cast<int>(iter2Diam * scale),
                                                             static_cast<int>(iter2Diam * scale));

                                    // Image big enough for just the overlapping parts
                                    // One body's origin will be in one corner, the other in the opposite
                                    int imgWidth = static_cast<int>(scale * fabs((iter1X - iter2X)));
                                    if (imgWidth <= 0) imgWidth = 1;
                                    int imgHeight = static_cast<int>(scale * fabs((iter1Y - iter2Y)));
                                    if (imgHeight <= 0) imgHeight = 1;

                                    // The sprite images are QImages with the cropped and correctly placed sprites,
                                    // with any leftover background set to transparent
                                    // The sprite images are then combined (into spriteImage1) using
                                    // QPainter::CompositionMode_SourceIn to only display overlapping parts of the images
                                    QImage spriteImage1(imgWidth, imgHeight, QImage::Format_ARGB32_Premultiplied);
                                    spriteImage1.fill(QColor(0,0,0,0));
                                    QImage spriteImage2(imgWidth, imgHeight, QImage::Format_ARGB32_Premultiplied);
                                    spriteImage2.fill(QColor(0,0,0,0));

                                    // Minimums will be the coordinates of the top left of the image
                                    // Taking away the minimums from the bodies' coordinates will produce
                                    // their correct new coordinates for the overlap image
                                    double minX = fmin(iter1X, iter2X);
                                    double minY = fmin(iter1Y, iter2Y);
                                    // Calculate the coordinates where the images need to be drawn
                                    double x1 = scale * (iter1X - minX);
                                    double y1 = scale * (iter1Y - minY);
                                    double x2 = scale * (iter2X - minX);
                                    double y2 = scale * (iter2Y - minY);

                                    // Draw sprites into the spriteImages
                                    QPainter p1(&spriteImage1);
                                    // If we are drawing a rocket we need to rotate it to the correct angle
                                    if ((*iter1)->getType() == Body::PlayerRocket) {
                                        // Save state of the painter
                                        p1.save();
                                        // Move the painter to where the rocket is going to be drawn
                                        // (Want the centre of rotation to be the centre of the rocket)
                                        p1.translate(x1, y1);
                                        p1.rotate(rocket->getAngle());
                                        p1.drawImage(static_cast<int>(-scale * (iter1Diam / 2.0)),
                                                     static_cast<int>(-scale * (iter1Diam / 2.0)),
                                                     sprite1.toImage());
                                        // Restore state of the painter
                                        p1.restore();
                                    } else {
                                        p1.drawImage(static_cast<int>(x1 - scale * (iter1Diam / 2.0)),
                                                     static_cast<int>(y1 - scale * (iter1Diam / 2.0)),
                                                     sprite1.toImage());
                                    }

                                    QPainter p2(&spriteImage2);
                                    if ((*iter2)->getType() == Body::PlayerRocket) {
                                        p2.save();
                                        p2.translate(x2, y2);
                                        p2.rotate(rocket->getAngle());
                                        p2.drawImage(static_cast<int>(-scale * (iter2Diam / 2.0)),
                                                     static_cast<int>(-scale * (iter2Diam / 2.0)),
                                                     sprite2.toImage());
                                        p2.restore();
                                    } else {
                                        p2.drawImage(static_cast<int>(x2 - scale * (iter2Diam / 2.0)),
                                                     static_cast<int>(y2 - scale * (iter2Diam / 2.0)),
                                                     sprite2.toImage());
                                    }
                                    // Draw the spriteImages on top of each other and find any overlap
                                    p1.setCompositionMode(QPainter::CompositionMode_SourceIn);
                                    p1.drawImage(0, 0, spriteImage2);

                                    uchar *line;
                                    collision = false;
                                    QRgb pixel;
                                    // Scan through the image, if any pixels are not ARGB=00000000 then there was
                                    // some overlap --> collision has occurred
                                    for (int i = 0, height = spriteImage1.height(); i < height && !collision; i++) {
                                        // Look at each line
                                        line = spriteImage1.scanLine(i);
                                        for (int j = 0, width = spriteImage1.width(); j < width; j++) {
                                            // Look at each pixel in line
                                            pixel = static_cast<QRgb>(line[static_cast<unsigned int>(j) * sizeof (QRgb)]);
                                            if (pixel != 00000000) {
                                                collision = true;
                                                if ((*iter1)->getType() == Body::PlayerRocket
                                                        || (*iter2)->getType() == Body::PlayerRocket) {
                                                }
                                                break;
                                            }
                                        }
                                    }
                                } else {
                                    collision = true;
                                }
                            }
                        }
                        // We are sure a collision has occurred --> Handle it
                        if (collision) {
                            // Rocket shouldn't combine, it should explode instead
                            if (mode == Exploration
                                    && ((*iter1)->getType() == Body::PlayerRocket
                                        || (*iter2)->getType() == Body::PlayerRocket)) {
                                std::cout << "ROCKET COLLISION" << std::endl;
                                rocket->setVel(0, 0);
                                rocket->setActive(false);
                                rocket->setExploding(true);
                            } else {
                                // Combine the two colliding bodies, and mark the smaller body for removal
                                if ((*iter1)->getMass() >= (*iter2)->getMass()) {
                                    (*iter1)->combine(*iter2);
                                    (*iter2)->setActive(false);
                                } else {
                                    (*iter2)->combine(*iter1);
                                    (*iter1)->setActive(false);
                                }
                            }
                        } else {
                            // Are the bodies even remotely close to each other?
                            if (fabs(iter1X - iter2X) + fabs(iter1Y - iter2Y) < 2000) {
                                // Some optimisations to make sure it's worthwhile to calculate the gravitational forces
                                // At very large distances the force is negligible
                                double iter1Mass = (*iter1)->getMass();
                                double iter2Mass = (*iter2)->getMass();
                                // If iter1 is a star and iter2 is an asteroid, the force put on iter1 is
                                // probably negligible and probably not worth calculating, so want the ratio
                                // to be smaller when iter1 is larger
                                double massRatio = iter2Mass / iter1Mass;
                                // If iter1 is NOT massively larger than iter2, continue
                                if (massRatio > 0.001) {
                                    double sqDist = iter1Pos.squareDist(iter2Pos);
//                                    std::cout << "sqDist: " << sqDist << ", iter1Mass: " << iter1Mass << ", iter2Mass: " << iter2Mass
//                                              << "\nmassRatio: " << massRatio << ", f: " << massRatio / sqDist
//                                              << "\n\n";
                                    // If the two bodies are relatively close, continue
                                    if (sqDist < 1000000) {
                                        // Calculate gravitational force exerted on body and update velocity
                                        // vel += (G * mass2 * (pos2 - pos 1)) / (dist ^ 3)
                                        // Converted to my Vector notation:
                                        // vel += (pos2 - po1).scale(G * mass2 / (dist ^ 3))
                                        double dist = iter2Pos.distance(iter1Pos);
                                        if (dist < 1) dist = 1;
                                        Vector iter2PosCopy;
                                        iter2PosCopy.set(iter2Pos);
                                        (*iter1)->setVel( // Set velocity
                                            (*iter1)->getVel().add( // Add following to current velocity
                                                (iter2PosCopy.sub(iter1Pos)).scale( // (pos2 - pos1) scaled by
                                                    G * (*iter2)->getMass() / // G * mass2 divided by
                                                        pow(dist, 3)) // (dist^3)
                                            )
                                        );
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (mode == Exploration && rocket->isActive()) {
                // W held down? --> Accelerate
                if (rocket->isFiring()) rocket->accelerate();
                // A held down? --> Rotate anti-clockwise
                if (rocket->isRotatingAntiCW()) rocket->rotate(-5);
                // D held down? --> Rotate clockwise
                if (rocket->isRotatingCW()) rocket->rotate(5);
            }
            for (std::list<Body*>::iterator iter = bodies.begin(), end = bodies.end(); iter != end; ++iter) {
                // Update position if the body is active
                if ((*iter)->isActive()) {
                    (*iter)->move();
                } else if ((*iter)->getType() != Body::PlayerRocket) {
                    // Remove body if it isn't active
                    mut.lock();
                    delete *iter;
                    iter = bodies.erase(iter);
                    mut.unlock();
                }
            }
        }
        // Sleep to maintain ~60 ticks per second
        //std::this_thread::sleep_for(std::chrono::milliseconds(16));
        tickEndTime = std::chrono::high_resolution_clock::now();
        tickElapsedTime = tickEndTime - tickStartTime;
        //std::cout << tickElapsedTime.count() << std::endl;
        while(tickElapsedTime.count() < 16.6) {
            //std::cout << "sleeping: " << tickElapsedTime.count() << std::endl << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            tickEndTime = std::chrono::high_resolution_clock::now();
            tickElapsedTime = tickEndTime - tickStartTime;
        }
        std::cout << "Ticks per second = " << 1000 / tickElapsedTime.count() << std::endl;
    }
}

/**
 * @brief Simulation::setG Sets the strength of gravity, G.
 * @param factor newGravity = factor * defaultGravity
 */
void Simulation::setG(double factor) {
    G = factor * G_DEFAULT;
}

/**
 * @brief Simulation::setVisibleRegion Updates the visible region of the player.
 * @param x The x-coordinate of the top left of the visible region
 * @param y The y-coordinate of the top elft of the visible region
 * @param newWidth The width of the visible region
 * @param newHeight The height of the visible region
 * @param newScaleThe new scale of the Simulation
 */
void Simulation::setVisibleRegion(double x, double y, double newWidth, double newHeight, double newScale) {
    visibleRegion->setRect(static_cast<int>(x),
                           static_cast<int>(y),
                           static_cast<int>(newWidth),
                           static_cast<int>(newHeight));
    // If we are zooming out, spawn more systems since we can see further now
    if (mode == Exploration && newScale < scale) {
        spawnPlanetarySystem();
    }
    scale = newScale;
}

/**
 * @brief Simulation::setPaused Sets whether the simulation should be paused.
 * @param b True if the simulation should be paused
 */
void Simulation::setPaused(bool b) {
    paused = b;
}

/**
 * @brief Simulation::getMode Returns the current mode of the simulation.
 * @return The current mode of the simulation
 */
int Simulation::getMode() {
    return mode;
}

/**
 * @brief Simulation::setMode Sets the current mode of the simulation.
 * @param newMode The new mode of the simulation
 */
void Simulation::setMode(Mode newMode) {
    mode = newMode;
}

/**
 * @brief Simulation::getRocket Returns the rocket being used in the current
 * simulation. The rocket is only active when the simulation is in
 * Exploration mode.
 * @return
 */
Rocket* Simulation::getRocket() {
    return rocket;
}

/**
 * @brief Simulation::setRocket Sets the user controlled rocket in the simulation
 * to the specified Rocket.
 * @param newRocket The new user controlled rocket in the simulation
 */
void Simulation::setRocket(Rocket *newRocket) {
    if (rocket) delete rocket;
    rocket = newRocket;
}

/**
 * @brief Simulation::calculateValidSpawningRegion Calculate valid region where we
 * can spawn planetary systems out of sight of the user. This is equal to the visible
 * region (zoomed out 1 level) with at least 1000 pixels each side of the visible
 * region.
 * @return A QRectF representing the valid region where planetary systems can
 * be spawned in Exploration mode
 */
QRectF Simulation::calculateValidSpawningRegion() {
    // Top left of the valid spawning region
    double minX = rocket->getX() - (visibleRegion->width() / 2) * 1.25;
    double minY = rocket->getY() - (visibleRegion->height() / 2) * 1.25;
    // Dimensions, zoomed out by one level
    double validRegionWidth = visibleRegion->width() * 1.25;
    double validRegionHeight = visibleRegion->height() * 1.25;
    // Make sure there is at least 1000 pixels between the visible
    // region and the valid spawning region
    if (fabs(visibleRegion->x() - minX) < 1000) {
        minX = visibleRegion->x() - 1000;
        validRegionWidth = visibleRegion->width() + 2000;
    }
    if (fabs(visibleRegion->y() - minY) < 1000) {
        minY = visibleRegion->y() - 1000;
        validRegionHeight = visibleRegion->height() + 2000;
    }
    return QRectF(minX, minY, validRegionWidth, validRegionHeight);
}

/**
 * @brief Simulation::getMap Gets the map representing the area explored
 * by the user-controlled rocket in Exploration mode.
 * @return A QImage representing the area explored by the user-controlled
 * rocket in Exploration mode.
 */
QImage* Simulation::getMap() {
    return exploredMap;
}














