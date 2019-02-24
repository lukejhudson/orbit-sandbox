#ifndef BODY_H
#define BODY_H

#include <QtGui>
#include <string>
#include "vector.h"
#include "sprites.h"

/*
 * Class for a generic body such as an asteroid or star.
 */
class Body {
public:
    Body();
    Body(double mass, double diam, Vector pos, Vector vel, int type);
    Body(double mass, double diam, Vector pos, Vector vel, int type, QImage img);
    Body(int type);
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
    void setType(int t);
    bool isActive();
    void setActive(bool b);
    void move(); // Add vel to pos
    void combine(Body b); // Combine two bodies together
    Body copy(); // Copies this Body
    std::string toString();
    bool operator==(const Body& b);
    QImage* getSprite();
    void setSprite(QImage img);
    void resizeSprite(Sprites sprites, double scale);

private:
    double mass;
    double diameter;
    Vector pos;
    Vector vel;
    int type; // What does the body represent? 0 = star, 1 = asteroid
    bool active; // Should the body interact with other bodies?
    QImage *sprite = nullptr;
};

#endif // BODY_H
