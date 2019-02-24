#include <string>
#include <math.h>
#include <iostream>
#include "body.h"

/**
 * @brief Body::Body Basic constructor. Initialises variables to invalid values.
 */
Body::Body() {
    mass = -1;
    diameter = -1;
    pos = new Vector(-1, -1);
    vel = new Vector(-1, -1);
    type = -1;
    active = true;
}

/**
 * @brief Body::Body Initialises variables to specified values.
 * @param mass Mass of body
 * @param diam Diameter of body
 * @param pos Position Vector of body
 * @param vel Velocity Vector of body
 * @param type Type of body (0 = star, 1 = asteroid, 2 = white dwarf, 3 = black hole)
 */
Body::Body(double mass, double diam, Vector pos, Vector vel, int type) {
    this->mass = mass;
    this->diameter = diam;
    this->pos = pos;
    this->vel = vel;
    this->type = type;
    active = true;
}

/**
 * @brief Body::Body Initialises variables to specified values, including the body's sprite.
 * @param mass Mass of body
 * @param diam Diameter of body
 * @param pos Position Vector of body
 * @param vel Velocity Vector of body
 * @param type Type of body (0 = star, 1 = asteroid, 2 = white dwarf, 3 = black hole)
 * @param img The body's sprite
 */
Body::Body(double mass, double diam, Vector pos, Vector vel, int type, QImage img) {
    this->mass = mass;
    this->diameter = diam;
    this->pos = pos;
    this->vel = vel;
    this->type = type;
    setSprite(img);
    active = true;
}

/**
 * @brief Body::Body Initialises variable to generic values for the specified type.
 * @param type Type of body (0 = star, 1 = asteroid, 2 = white dwarf, 3 = black hole)
 */
Body::Body(int type) {
    // Specific mass and diameter for each type
    switch (type) {
        case 0: // Star
            mass = 1000;
            diameter = 50;
            break;
        case 1: // Asteroid
            mass = 5;
            diameter = 5;
            break;
        case 2: // White dwarf
            mass = 1000;
            diameter = 10;
            break;
        case 3: // Black hole
            mass = 5000;
            diameter = 50;
            break;
    }
    // Generic information
    pos = new Vector(-1, -1);
    vel = new Vector(-1, -1);
    this->type = type;
    active = true;
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
Vector Body::getPos() const {
    return pos;
}

/**
 * @brief Body::getX
 * @return The x-coordinate of the body
 */
double Body::getX() {
    return pos.getX();
}

/**
 * @brief Body::getY
 * @return The y-coordinate of the body
 */
double Body::getY() {
    return pos.getY();
}

/**
 * @brief Body::getVel
 * @return The velocity Vector of the body
 */
Vector Body::getVel() const {
    return vel;
}

/**
 * @brief Body::getVelX
 * @return The x-velocity of the body
 */
double Body::getVelX() {
    return vel.getX();
}

/**
 * @brief Body::getVelY
 * @return The y-velocity of the body
 */
double Body::getVelY() {
    return vel.getY();
}

/**
 * @brief Body::setPos
 * @param v New position Vector of the body
 */
void Body::setPos(Vector v) {
    pos = v;
}

/**
 * @brief Body::setPos
 * @param x New x-coordinate of the body
 * @param y New y-coordinate of the body
 */
void Body::setPos(double x, double y) {
    pos.setX(x);
    pos.setY(y);
}

/**
 * @brief Body::setX
 * @param x New x-coordinate of the body
 */
void Body::setX(double x) {
    pos.setX(x);
}

/**
 * @brief Body::setY
 * @param y New y-coordinate of the body
 */
void Body::setY(double y) {
    pos.setY(y);
}

/**
 * @brief Body::setVel
 * @param v New velocity Vector of the body
 */
void Body::setVel(Vector v) {
    vel = v;
}

/**
 * @brief Body::setVel
 * @param x New x-velocity of the body
 * @param y New y-velocity of the body
 */
void Body::setVel(double x, double y) {
    vel.setX(x);
    vel.setY(y);
}

/**
 * @brief Body::setVelX
 * @param x New x-velocity of the body
 */
void Body::setVelX(double x) {
    vel.setX(x);
}

/**
 * @brief Body::setVelY
 * @param y New y-velocity of the body
 */
void Body::setVelY(double y) {
    vel.setY(y);
}

/**
 * @brief Body::getType
 * @return The type of the body (0 = star, 1 = asteroid)
 */
int Body::getType() {
    return type;
}

/**
 * @brief Body::setType
 * @param t New type of the body (0 = star, 1 = asteroid)
 */
void Body::setType(int t) {
    type = t;
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
    pos.add(vel);
}

/**
 * @brief Body::combine Combines two bodies when they collide.
 * @param b The Body to combine to this Body
 */
void Body::combine(Body b) {
    double bMass = b.getMass();
    std::cout << "Diam before: " << diameter;
    mass += b.getMass();
    diameter = hypot(diameter, b.getDiameter());
    double massRatio = bMass / (bMass + mass);
    vel.add(b.getVel().scale(massRatio));
    std::cout << "   Diam after: " << diameter << std::endl;


    // pi r1^2 + pi r2^2 = pi r3^2
    // d1^2 + d2^2 = d3^2

    // Doesn't take into account the angle that they collide

}

/**
 * @brief Body::copy Returns a copy of this Body.
 * @return A copy of this Body
 */
Body Body::copy() {
    return *new Body(mass, diameter, pos.copy(), vel.copy(), type);
}

/**
 * @brief Body::toString Returns a string representation of the body,
 * including its position and velocity vectors.
 * @return A string representation of the body
 */
std::string Body::toString() {
    return "pos: " + pos.toString() + ", vel: " + vel.toString();
}

/**
 * @brief operator == Compares two bodies and returns whether or not lhs == rhs.
 * @param lhs The first Body to compare
 * @param rhs The second Body to compare
 * @return True if the two bodies are the sames
 */
bool Body::operator==(const Body& rhs) {
    return this->getPos().equals(rhs.getPos())
            && this->getVel().equals(rhs.getVel());
}

/**
 * @brief Body::getSprite Returns this Body's sprite.
 * @return
 */
QImage* Body::getSprite() {
    //std::cout << "getSprite: " << sprite->isNull() << std::endl;
    return sprite;
}

/**
 * @brief Body::setSprite Sets this Body's sprite.
 * @param sprite The new sprite for the Body.
 */
void Body::setSprite(QImage img) {
    std::cout << "setSprite: " << img.isNull() << std::endl;
    if (sprite != nullptr) {
        delete sprite;
    }
    sprite = new QImage(img);
    std::cout << "setSprite2: " << sprite->isNull() << std::endl;
}

/**
 * @brief Body::resizeSprite
 * @param sprites
 * @param scale
 */
void Body::resizeSprite(Sprites sprites, double scale) {
    switch (type) {
        case 0: // Star
            setSprite(sprites.starImage.scaled(static_cast<int>(scale * diameter),
                                               static_cast<int>(scale * diameter)));
            break;
        case 1: // Asteroid
            setSprite(sprites.asteroidImage.scaled(static_cast<int>(scale * diameter),
                                                   static_cast<int>(scale * diameter)));
            break;
        case 2: // White dwarf
            setSprite(sprites.whitedwarfImage.scaled(static_cast<int>(scale * diameter),
                                                     static_cast<int>(scale * diameter)));
            break;
        case 3: // Black hole
            setSprite(sprites.blackholeImage.scaled(static_cast<int>(scale * diameter),
                                                    static_cast<int>(scale * diameter)));
            break;
    }
}
















