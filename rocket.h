#ifndef ROCKET_H
#define ROCKET_H

#include "body.h"

class Rocket : public Body {
public:
    Rocket();
    Rocket(Vector pos, Vector vel);
    Rocket(Vector pos, Vector vel, bool firing, bool exploding,
           int explodingCount, bool rotatingAntiCW, bool rotatingCW,
           int angle);
    virtual ~Rocket();

    bool isFiring();
    void setFiring(bool firing);
    bool isExploding();
    void setExploding(bool exploding);
    int getExplodingCount();
    void incrementExplodingCount();
    bool isRotatingAntiCW();
    void setRotatingAntiCW(bool rotating);
    bool isRotatingCW();
    void setRotatingCW(bool rotating);
    int getAngle();
    void setAngle(int angle);
    void rotate(int angle);
    void accelerate();
    Rocket* copy();

private:
    // Are the rocket's engines firing?
    bool firing = false;
    // Has the rocket collided with another body and is now exploding?
    bool exploding = false;
    // Which frame of the explosion animation are we on?
    int explodingCount = 0;
    // Is the rocket rotating anti-clockwise?
    bool rotatingAntiCW = false;
    // Is the rocket rotating clockwise?
    bool rotatingCW = false;

    // Angle the rocket is facing, 0 = up
    int angle = 0;

};

#endif // ROCKET_H
