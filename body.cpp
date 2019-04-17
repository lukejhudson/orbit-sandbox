#include <string>
#include <cmath>
#include <iostream>
#include "body.h"

/**
 * @brief Body::Body Basic constructor. Initialises variables to invalid values.
 */
Body::Body() {
    mass = -1;
    diameter = -1;
    pos = nullptr;
    vel = nullptr;
    type = Asteroid;
    active = true;
}

/**
 * @brief Body::Body Initialises variables to specified values.
 * @param mass Mass of body
 * @param diam Diameter of body
 * @param pos Position Vector of body
 * @param vel Velocity Vector of body
 * @param type Type of body (see Body::BodyType)
 */
Body::Body(double mass, double diam, Vector *pos, Vector *vel, BodyType type, int planetType) {
    this->mass = mass;
    this->diameter = diam;
    this->pos = pos;
    this->vel = vel;
    this->type = type;
    this->planetType = planetType;
    active = true;
}

/**
 * @brief Body::Body Initialises variable to generic values for the specified type.
 * @param type Type of body
 */
Body::Body(BodyType type) {
    // Specific mass and diameter for each type
    int rand;
    planetType = 0;
    switch (type) {
        default:
        case Asteroid:
            // Mass and diam = 2 +/- 1
            rand = 1 + std::rand() % 3;
            mass = rand;
            diameter = rand;
            break;
        case Planet:
            // Mass = 150 +/- 100
            mass = 150 + std::rand() % 101;
            // Diam = 10 +/- 5
            diameter = 5 + std::rand() % 11;
            planetType = std::rand() % 5 + 1; // Random from 1 to 5
            break;
        case Star:
            // Mass = 9000 +/- 1000
            mass = 8000 + std::rand() % 2001;
            // Diam = 50 +/- 10
            diameter = 40 + std::rand() % 21;
            break;
        case WhiteDwarf:
            // Mass = 9000 +/- 1000
            mass = 8000 + std::rand() % 2001;
            // Diam = 10 +/- 5
            diameter = 5 + std::rand() % 11;
            break;
        case BlackHole:
            // Mass = 30000 +- 5000
            mass = 25000 + std::rand() % 10001;
            // Diam = 25 +/- 5
            diameter = 20 + std::rand() % 11;
            break;
    }
    //std::cout << "Type: " << type << "\tMass: " << mass << "\tDiam: " << diameter << std::endl;
    // Generic information
    pos = new Vector(-1, -1);
    vel = new Vector(-1, -1);
    this->type = type;
    active = true;
}

/**
 * @brief Body::~Body Destructor.
 */
Body::~Body() {
    delete pos;
    delete vel;
}

/**
 * @brief Body::getMass
 * @return The mass of the body
 */
double Body::getMass() {
    return mass;
}

/**
 * @brief Body::setMass
 * @param m The new mass of the body
 */
void Body::setMass(double m) {
    mass = m;
}

/**
 * @brief Body::getDiameter
 * @return The diameter of the body
 */
double Body::getDiameter() {
    return diameter;
}

/**
 * @brief Body::setDiameter
 * @param d The new mass of the body
 */
void Body::setDiameter(double d) {
    diameter = d;
}

/**
 * @brief Body::getPos
 * @return The position Vector of the body
 */
Vector* Body::getPos() const {
    return pos;
}

/**
 * @brief Body::getX
 * @return The x-coordinate of the body
 */
double Body::getX() {
    return pos->getX();
}

/**
 * @brief Body::getY
 * @return The y-coordinate of the body
 */
double Body::getY() {
    return pos->getY();
}

/**
 * @brief Body::getVel
 * @return The velocity Vector of the body
 */
Vector* Body::getVel() const {
    return vel;
}

/**
 * @brief Body::getVelX
 * @return The x-velocity of the body
 */
double Body::getVelX() {
    return vel->getX();
}

/**
 * @brief Body::getVelY
 * @return The y-velocity of the body
 */
double Body::getVelY() {
    return vel->getY();
}

/**
 * @brief Body::setPos
 * @param v New position Vector of the body
 */
void Body::setPos(Vector *v) {
    pos = v;
}

/**
 * @brief Body::setPos
 * @param x New x-coordinate of the body
 * @param y New y-coordinate of the body
 */
void Body::setPos(double x, double y) {
    pos->setX(x);
    pos->setY(y);
}

/**
 * @brief Body::setX
 * @param x New x-coordinate of the body
 */
void Body::setX(double x) {
    pos->setX(x);
}

/**
 * @brief Body::setY
 * @param y New y-coordinate of the body
 */
void Body::setY(double y) {
    pos->setY(y);
}

/**
 * @brief Body::setVel
 * @param v New velocity Vector of the body
 */
void Body::setVel(Vector *v) {
    vel = v;
}

/**
 * @brief Body::setVel
 * @param x New x-velocity of the body
 * @param y New y-velocity of the body
 */
void Body::setVel(double x, double y) {
    vel->setX(x);
    vel->setY(y);
}

/**
 * @brief Body::setVelX
 * @param x New x-velocity of the body
 */
void Body::setVelX(double x) {
    vel->setX(x);
}

/**
 * @brief Body::setVelY
 * @param y New y-velocity of the body
 */
void Body::setVelY(double y) {
    vel->setY(y);
}

/**
 * @brief Body::getType
 * @return The type of the body (see Body::BodyType)
 */
int Body::getType() {
    return type;
}

/**
 * @brief Body::setType
 * @param t New type of the body (see Body::BodyType)
 */
void Body::setType(BodyType t) {
    type = t;
}

/**
 * @brief Body::getplanetType
 * @return The type of the planet, i.e. which sprite it should take
 */
int Body::getPlanetType() {
    return planetType;
}

/**
 * @brief Body::setType
 * @param t New type of the planet, i.e. which sprite it should take
 */
void Body::setPlanetType(int t) {
    planetType = t;
}

/**
 * @brief Body::isActive Returns whether or not the body is active and
 * therefore if it is able to interact with other bodies.
 * @return True if the body is active
 */
bool Body::isActive() {
    return active;
}

/**
 * @brief Body::setActive Sets whether the body should be able to interact
 * with other bodies. A body that is inactive will be removed from the
 * simulation in the next tick.
 * @param b Should the body be able to interact with others?
 */
void Body::setActive(bool b) {
    active = b;
}

/**
 * @brief Body::move Move the body by one step by adding its velocity
 * Vector to its position Vector.
 */
void Body::move() {
    pos->add(vel);
}

/**
 * @brief Body::combine Combines two bodies when they collide.
 * @param b The Body to combine to this Body
 */
void Body::combine(Body *b) {
    // Grow size proportionally to the size of the body consumed
    //diameter = hypot(diameter, b->getDiameter());
//    std::cout << "Calculation: diam *= 1 + (b->getMass() / diameter) / (mass / diameter)\n"
//              << diameter << " *= 1 + (" << b->getMass() << " / " << diameter << ") / (" << mass << " / " << diameter << ") = 1 + " << (b->getMass() / diameter) / (mass / diameter) << std::endl;
    diameter *= 1 + (0.5 * b->getMass() / diameter) / (mass / diameter);
    //std::cout << "Diam = " << diameter << std::endl << std::endl;
    // 2x 1D momentum calculations
    // m1v1 + m2v2 = m3v3 = (m1+m2)v3
    // v3 = (m1v1 + m2v2) / (m1 + m2)
    double vx, vy, bMass = b->getMass();
    vx = (mass * vel->getX() + bMass * b->getVelX()) /
            (mass + bMass);
    vy = (mass * vel->getY() + bMass * b->getVelY()) /
            (mass + bMass);
    vel->set(vx, vy);
    // Consume mass
    mass += bMass;
}

/**
 * @brief Body::copy Returns a copy of this Body.
 * @return A copy of this Body
 */
Body* Body::copy() {
    return new Body(mass, diameter, pos->copy(), vel->copy(), type, planetType);
}

/**
 * @brief Body::toString Returns a string representation of the body,
 * including its position and velocity vectors.
 * @return A string representation of the body
 */
std::string Body::toString() {
    return "pos: " + pos->toString() + ", vel: " + vel->toString();
}

/**
 * @brief operator == Compares two bodies and returns whether or not lhs == rhs.
 * @param lhs The first Body to compare
 * @param rhs The second Body to compare
 * @return True if the two bodies are the sames
 */
bool Body::operator==(const Body& rhs) {
    return this->getPos()->equals(rhs.getPos())
            && this->getVel()->equals(rhs.getVel());
}

/**
 * @brief Body::isWithin Returns true if any part of the Body is within
 * the given area.
 * @param v The Vector representing the top left of the area
 * @param width The width of the area
 * @param height The height of the area
 * @return True if the Body is within the area
 */
bool Body::isWithin(QRect rect) {
    double radius = diameter / 2.0;
    double posX = pos->getX(), posY = pos->getY();

    return (posX + radius >= rect.x()
            && posX - radius <= rect.x() + rect.width()
            && posY + radius >= rect.y()
            && posY - radius <= rect.y() + rect.height());
}















