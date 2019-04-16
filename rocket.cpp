#include <cmath>
#include <iostream>
#include "rocket.h"

#define MASS 5
#define DIAMETER 5

/**
 * @brief Rocket::Rocket Create a new rocket with uninitialised position and
 * velocity Vectors.
 */
Rocket::Rocket() {
    mass = MASS;
    diameter = DIAMETER;
    pos = new Vector(-1, -1);
    vel = new Vector(-1, -1);
    type = PlayerRocket;
    active = true;
}

/**
 * @brief Rocket::Rocket Create a new rocket with specified position and
 * velocity Vectors.
 * @param pos The rocket's position Vector
 * @param vel The rocket's velocity Vector
 */
Rocket::Rocket(Vector *pos, Vector *vel) {
    mass = MASS;
    diameter = DIAMETER;
    this->pos = pos;
    this->vel = vel;
    type = PlayerRocket;
    active = true;
}


Rocket::Rocket(Vector *pos, Vector *vel, bool firing, bool exploding,
               int explodingCount, bool rotatingAntiCW, bool rotatingCW,
               int angle) {
    mass = MASS;
    diameter = DIAMETER;
    this->pos = pos;
    this->vel = vel;
    type = PlayerRocket;
    active = true;
    this->firing = firing;
    this->exploding = exploding;
    this->explodingCount = explodingCount;
    this->rotatingAntiCW = rotatingAntiCW;
    this->rotatingCW = rotatingCW;
    this->angle = angle;
}



/**
 * @brief Rocket::isFiring Are the rocket's engines firing?
 * @return True if the rocket's engines are firing
 */
bool Rocket::isFiring() {
    return firing;
}

/**
 * @brief Rocket::setFiring Sets whether the rocket's engines are firing.
 * @param firing True if the rocket's engines should be firing
 */
void Rocket::setFiring(bool firing) {
    this->firing = firing;
}

/**
 * @brief Rocket::isExploding Has the rocket collided with another body
 * and is now exploding?
 * @return True if the rocket is exploding
 */
bool Rocket::isExploding() {
    return exploding;
}

/**
 * @brief Rocket::setExploding Sets whether the rocket is exploding.
 * @param firing True if the rocket should be exploding
 */
void Rocket::setExploding(bool exploding) {
    this->exploding = exploding;
}

/**
 * @brief Rocket::getExplodingCount Returns the frame of the explosion
 * animation that should be shown.
 * @return The frame of the explosion animation that should be shown
 */
int Rocket::getExplodingCount() {
    return explodingCount;
}

/**
 * @brief Rocket::incrementExplodingCount Increments the exploding count,
 * causing the next frame of the explosion animation to be shown.
 */
void Rocket::incrementExplodingCount() {
    explodingCount++;
}

/**
 * @brief Rocket::isRotatingAntiCW Is the rocket rotating anti-clockwise?
 * @return True if the rocket is rotating anti-clockwise
 */
bool Rocket::isRotatingAntiCW() {
    return rotatingAntiCW;
}

/**
 * @brief Rocket::setRotatingAntiCW Sets whether the rocket is rotating
 * anti-clockwise.
 * @param rotating True if the rocket should be rotating anti-clockwise
 */
void Rocket::setRotatingAntiCW(bool rotating) {
    rotatingAntiCW = rotating;
    if (rotating) rotatingCW = false;
}

/**
 * @brief Rocket::isRotatingCW Is the rocket rotating clockwise?
 * @return True if the rocket is rotating clockwise
 */
bool Rocket::isRotatingCW() {
    return rotatingCW;
}

/**
 * @brief Rocket::setRotatingCW Sets whether the rocket is rotating
 * clockwise.
 * @param rotating True if the rocket should be rotating clockwise
 */
void Rocket::setRotatingCW(bool rotating) {
    rotatingCW = rotating;
    if (rotating) rotatingAntiCW = false;
}

/**
 * @brief Rocket::getAngle Returns the current angle of the rocket.
 * The angle is measured in degrees, where 0 = 360 = facing straight up.
 * 0 <= angle < 360
 * @return
 */
int Rocket::getAngle() {
    return angle;
}

/**
 * @brief Rocket::setAngle Sets the angle of the rocket.
 * The angle is measured in degrees, where 0 = 360 = facing straight up.
 * 0 <= angle < 360
 * @param newAngle The new angle of the rocket
 */
void Rocket::setAngle(int newAngle) {
    if (newAngle < 0 || newAngle >= 360)
        newAngle = newAngle % 360;
    angle = newAngle;
    if (angle >= 360) angle -= 360;
    if (angle < 0) angle += 360;
}

/**
 * @brief Rocket::rotate Rotates the rocket by the specified amount. The
 * current angle of the rocket is incremented by the given angle.
 *  * The angle is measured in degrees, where 0 = 360 = facing straight up.
 * 0 <= angle < 360
 * @param rotation The angle to increment the rocket's angle by
 */
void Rocket::rotate(int rotation) {
    if (rotation < 0 || rotation >= 360)
        rotation = rotation % 360;
    angle += rotation;
    if (angle >= 360) angle -= 360;
    if (angle < 0) angle += 360;
}

/**
 * @brief Rocket::accelerate If the rocket is firing its engines, accelerate
 * the rocket in its current direction.
 */
void Rocket::accelerate() {
    if (firing) {
        // Scalar velocity to add to the rocket
        double velocity = 0.01;
        Vector velocityVector;
        // Convert angle to radians
        double angleRadians = angle * M_PI / 180.0;
        // Using trig, 0 degrees pointing upwards:
        // x component = v * sin (angleRadians)
        velocityVector.setX(velocity * sin(angleRadians));
        // y component = -v * cos (angleRadians)
        // y component must be flipped since 0 degrees points north, but y increases
        // downwards
        velocityVector.setY(-velocity * cos(angleRadians));

        // Add new velocity vector to current velocity vector
        vel->add(&velocityVector);
    }
}


/**
 * @brief Rocket::copy Returns a copy of this Rocket.
 * @return A copy of this Rocket
 */
Rocket* Rocket::copy() {
    return new Rocket(pos->copy(), vel->copy(), firing, exploding,
                      explodingCount, rotatingAntiCW, rotatingCW,
                      angle);
}




























