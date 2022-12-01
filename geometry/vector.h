#ifndef CPP_VECTOR_H
#define CPP_VECTOR_H

#include <cmath>

const double eps = 1e-5;

bool equal (double a, double b) {
    return std::fabs(a - b) < eps;
}


struct vector {
    vector() = default;
    vector(const double x, const double y): x(x), y(y) {}
    ~vector() = default;

    vector& operator+= (const vector& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    vector& operator-= (const vector& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    vector& operator*= (const double scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    vector& operator/= (const double scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    vector operator- () const { return vector(-x, -y); }
    vector operator+ () const { return vector(x, y); }

    double length() const { return std::sqrt(x * x + y * y); }

    double x;
    double y;
};

bool operator== (const vector& first, const vector& second) {
    return equal(first.x, second.x) && equal(first.y, second.y);
}

bool operator!= (const vector& first, const vector& second) { return !(first == second); }

vector operator+ (const vector& first, const vector& second) { return vector(first) += second; }
vector operator- (const vector& first, const vector& second) { return vector(first) -= second; }
vector operator* (const vector& first, const double second) { return vector(first) *= second; }
vector operator* (const double first, const vector& second) { return vector(second) *= first; }
vector operator/ (const vector& first, const double second) { return vector(first) /= second; }

double dot_product (const vector& lhs, const vector& rhs) { return lhs.x * rhs.x + lhs.y * rhs.y; }
double cross_product (const vector& lhs, const vector& rhs) { return lhs.x * rhs.y - lhs.y * rhs.x; }
bool collinear (const vector& lhs, const vector& rhs) { return equal(cross_product(lhs, rhs), 0.); }

#endif //CPP_VECTOR_H
