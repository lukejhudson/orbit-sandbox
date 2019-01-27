#include <math.h>
#include <string>
#include "vector.h"

#define EPSILON 1.0E-10

/**
 * @brief Vector::Vector Basic constructor. Initialises x and y to -1.
 */
Vector::Vector() {
    x = -1;
    y = -1;
}

/**
 * @brief Vector::Vector Creates a new Vector with the specified x and y values.
 * @param x The Vector's x value
 * @param y The Vector's y value
 */
Vector::Vector(double x, double y) {
    this->x = x;
    this->y = y;
}

/**
 * @brief Vector::Vector Creates a new Vector with x and y values equal to those
 * in the given Vector v.
 * @param v The Vector to extract x and y values from
 */
Vector::Vector(Vector *v) {
    x = v->getX();
    y = v->getY();
}

/**
 * @brief Vector::getX Returns the x value
 * @return The current x value
 */
double Vector::getX() {
    return x;
}

/**
 * @brief Vector::getY Returns the y value
 * @return The current y value
 */
double Vector::getY() {
    return y;
}

/**
 * @brief Vector::setX Sets the x value
 * @param x The new x value
 */
void Vector::setX(double x) {
    this->x = x;
}

/**
 * @brief Vector::setY Sets the y value
 * @param y The new y value
 */
void Vector::setY(double y) {
    this->y = y;
}

/**
 * @brief Vector::set Sets both the x and y values
 * @param x The new x value
 * @param y The new y value
 */
void Vector::set(double x, double y) {
    this->x = x;
    this->y = y;
}

/**
 * @brief Vector::set Sets both the x and y values to those in the given Vector.
 * @param v The Vector to copy the x and y values from
 */
void Vector::set(Vector v) {
    x = v.getX();
    y = v.getY();
}

/**
 * @brief Vector::dot Calculates the dot product of this Vector and the Vector v.
 * @param v  The Vector to calculate the dot product with
 * @return The dot product of this Vector and the Vector v
 */
double Vector::dot(Vector v) {
    return (x * v.getX()) + (y * v.getY());
}

/**
 * @brief Vector::scale Scale the vector by scale factor s.
 * @param s The scale factor to scale this Vector by
 * @return This Vector with the updated x and y values
 */
Vector Vector::scale(double s) {
    x *= s;
    y *= s;
    return this;
}

/**
 * @brief Vector::squareDist Find the distance between this Vector and
 * another given Vector v.
 * @param v The Vector to calculate the distance between
 * @return The distance between this Vector and the Vector v
 */
double Vector::distance(Vector v) {
    return hypot(x - v.getX(), y - v.getY());
}

/**
 * @brief Vector::squareDist Find the squared distance between this Vector and
 * another given Vector v.
 * @param v The Vector to calculate the squared distance between
 * @return The squared distance between this Vector and the Vector v
 */
double Vector::squareDist(Vector v) {
    return pow(x - v.getX(), 2) + pow(y - v.getY(), 2);
}

/**
 * @brief Vector::getNormal Calculates the scalar distance represented by this
 * Vector. This function is costly due to its use of square root.
 * @return The scalar distance represented by this Vector
 */
double Vector::getNormal() {
    return hypot(x, y);
}

/**
 * @brief Vector::getSquareNorm Calculates the scalar distance represented by
 * this Vector. This function is much more efficient than getNormal().
 * @return The squared scalar distance represented by this Vector
 */
double Vector::getSquareNorm() {
    return (x * x) + (y * y);
}

/**
 * @brief isEqual Compare equality of two doubles (within certain EPSILON).
 * Based on :
 * https://stackoverflow.com/questions/17333/what-is-the-most-effective-way-for-float-and-double-comparison
 * @param a First double to compare
 * @param b Second double to compare
 * @return True if the two double are equal (within certain EPSILON)
 */
bool isEqual(double a, double b) {
    return fabs(a - b) <= ((fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * EPSILON);
}

/**
 * @brief Vector::unitVector Transforms this Vector into its unit vector.
 */
void Vector::unitVector() {
    double d = getNormal();
    if (!isEqual(d, 0)) {
        x /= d;
        y /= d;
    }
}

/**
 * @brief Vector::add Adds this Vector to the given Vector v.
 * @param v The Vector to add to this Vector
 * @return This Vector with the updated x and y values
 */
Vector Vector::add(Vector v) {
    x += v.getX();
    y += v.getY();
    return this;
}

/**
 * @brief Vector::sub Subtracts the given Vector v from this Vector.
 * @param v The Vector to subtract from this Vector
 * @return This Vector with the updated x and y values
 */
Vector Vector::sub(Vector v) {
    x -= v.getX();
    y -= v.getY();
    return this;
}

/**
 * @brief Vector::equals Checks whether this Vector and the given Vector v are equal.
 * @param v The Vector to check against
 * @return True if the two Vectors are equal
 */
bool Vector::equals(Vector v) {
    return isEqual(x, v.getX()) && isEqual(y, v.getY());
}

/**
 * @brief Vector::copy Returns a copy of this Vector.
 * @return A copy of this Vector
 */
Vector Vector::copy() {
    return new Vector(x, y);
}

/**
 * @brief Vector::toString Creates a string representation of this Vector.
 * @return A string representation of this Vector
 */
std::string Vector::toString() {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
}
