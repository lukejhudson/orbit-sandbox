#ifndef BODY_H
#define BODY_H

#include <string>
#include "vector.h"

/*
 * Class for a generic body such as an asteroid or star.
 */
class Body {
public:
    Body();
    Body(double mass, double diam, Vector pos, Vector vel, int type);
    double getMass();
    double getDiameter();
    Vector getPos();
    double getX();
    double getY();
    Vector getVel();
    double getVelX();
    double getVelY();
    void setPos(Vector v);
    void setPos(double x, double y);
    void setX(double x);
    void setY(double y);
    void setVel(Vector v);
    void setVel(double x, double y);
    void setVelX(double x);
    void setVelY(double y);
    int getType();
    void setType(int t);
    void move(); // Add vel to pos
    std::string toString();

private:
    double mass;
    double diameter;
    Vector pos;
    Vector vel;
    int type; // What does the body represent? 0 = star, 1 = asteroid
};

#endif // BODY_H
