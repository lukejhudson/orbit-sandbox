#ifndef BODY_H
#define BODY_H

#include <string>
#include <QRect>
#include "vector.h"

/*
 * Class for a generic body such as an asteroid or star.
 */
class Body {

public:

    enum BodyType {
        Asteroid = 0,
        Planet = 1,
        Star = 2,
        WhiteDwarf = 3,
        BlackHole = 4,
        PlanetarySystem = 5,
        PlayerRocket = 6
    };

    Body();
    Body(double mass, double diam, Vector pos, Vector vel, BodyType type, int planetType);
    Body(BodyType);
    virtual ~Body();

    double getMass();
    void setMass(double m);
    double getDiameter();
    void setDiameter(double d);
    Vector getPos() const;
    double getX();
    double getY();
    Vector getVel() const;
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
    void setType(BodyType);
    int getPlanetType();
    void setPlanetType(int t);
    bool isActive();
    void setActive(bool b);
    void move(); // Add vel to pos
    void combine(Body *b); // Combine two bodies together
    Body* copy(); // Copies this Body
    std::string toString();
    bool operator==(const Body& b);
    bool isWithin(QRect rect);

protected:
    double mass;
    double diameter;
    Vector pos;
    Vector vel;
    BodyType type; // What does the body represent?
    bool active; // Should the body interact with other bodies?

private:
    int planetType; // Which planet sprite should the body (planet) have?
};

#endif // BODY_H
