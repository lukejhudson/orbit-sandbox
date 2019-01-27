#ifndef VECTOR_H
#define VECTOR_H

#include <string>

class Vector {
public:
    Vector();
    Vector(double x, double y);
    Vector(Vector *v);
    double getX();
    double getY();
    void setX(double x);
    void setY(double y);
    void set(double x, double y);
    void set(Vector v);
    double dot(Vector v); // Dot product
    Vector scale(double s); // Scale vector by factor s
    double distance(Vector v); // Distance from vector to v
    double squareDist(Vector v); // Squared distance (no sqrt)
    double getNormal(); // Distance of vector (with sqrt)
    double getSquareNorm(); // Squared distance of vector (no sqrt)
    void unitVector(); // Transform to unit vector
    Vector add(Vector v); // Add 2 vectors
    Vector sub(Vector v); // Sub 2 vectors
    bool equals(Vector v); // This vector == v?
    Vector copy(); // Returns a copy of this Vector
    std::string toString(); // std::string form of vector

private:
    double x;
    double y;
};

#endif // VECTOR_H
