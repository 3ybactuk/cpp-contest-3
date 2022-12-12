#ifndef CPP_GEOMETRY_H
#define CPP_GEOMETRY_H

#include "vector.h"
#include <vector>
#include <algorithm>
#include <cmath>

constexpr double pi() { return std::atan(1) * 4; }

size_t mod(const size_t& a, const size_t& b) {
    return (a % b + b) % b;
}

bool is_clockwise(const std::vector<vector>& coords) {
    double sum = 0;
    for (size_t i = 0; i < coords.size(); ++i) {
        size_t k = coords.size();
        sum += (coords[(i + 1) % k].x - coords[i].x) * (coords[(i + 1) % k].y + coords[i].y);
    }
    return sum > 0;
}

class shape {
public:
    virtual ~shape () = default;

    virtual vector center() const = 0;
    virtual double perimeter() const = 0;
    virtual double area() const = 0;
    virtual bool congruent_to(const shape& another) const = 0;

    virtual bool operator==(const shape& another) const = 0;
    virtual bool operator!=(const shape& another) const = 0;

    virtual void rotate(double angle) = 0;
    virtual void scale(double coefficient) = 0;
    virtual void translate(vector transform) = 0;
};

class circle: public shape {
public:
    circle(const vector& center, const double radius): _center(center), _radius(radius) {}
    circle(const circle& other) = default;
    circle& operator=(const circle& other) = default;
    ~circle() = default;

    vector center() const override { return _center; }

    double perimeter() const override { return 2 * pi() * _radius; }

    double area() const override { return pi() * _radius * _radius; }

    bool congruent_to(const shape& another) const override { return false; }

    bool congruent_to(const circle& another) const {
        return equal(_radius, dynamic_cast<const circle&>(another)._radius);
    }

    bool operator==(const circle& another) const {
        return (equal(another._radius, _radius) && another._center == _center);
    }

    bool operator!=(const circle& another) const { return !(*this == another); }

    bool operator==(const shape& another) const override { return false; }
    bool operator!=(const shape& another) const override { return true; }

    void rotate(double angle) override {} /// Does nothing. It's a circle

    void scale(double coefficient) override { _radius *= std::abs(coefficient); }

    void translate(vector transform) override {
        _center.x += transform.x;
        _center.y += transform.y;
    }

    double radius() const { return _radius; }

protected:
    vector _center;
    double _radius;
};

class polygon: public shape {
public:
    polygon() = default;

    polygon(const std::vector<vector>& points): _points(points) {
        if (is_clockwise(_points)) { std::reverse(_points.begin(), _points.end()); }
    }

    polygon(const polygon& other) = default;
    polygon& operator=(const polygon& other) = default;
    ~polygon() = default;

    vector center() const override {
        vector C(0, 0);

        for (size_t i = 0; i <= _points.size() - 1; ++i) {
            size_t k = _points.size();
            C.x += (_points[i].x + _points[(i + 1) % k].x) * cross_product(_points[i], _points[(i + 1) % k]);
            C.y += (_points[i].y + _points[(i + 1) % k].y) * cross_product(_points[i], _points[(i + 1) % k]);
        }

        double A = area();
        C.x = C.x / (6 * A);
        C.y = C.y / (6 * A);

        return C;
    }

    double perimeter() const override {
        double p_sum = 0;

        for (size_t i = 1; i <= _points.size(); ++i) {
            p_sum += (_points[i % _points.size()] - _points[i - 1]).length();
        }

        return p_sum;
    }

    double area() const override {
        double s_sum = 0;

        for (size_t i = 1; i <= _points.size(); ++i) {
            s_sum += cross_product(_points[i % _points.size()], _points[i - 1]);
        }

        return std::abs(s_sum) / 2.0;
    }

    bool congruent_to(const circle& another) const { return false; }

    bool congruent_to(const shape& another) const override {
        const polygon& copy = dynamic_cast<const polygon&>(another);
        if (_points.size() != copy._points.size()) { return false; }

        size_t k = _points.size();
        std::vector<std::pair<size_t, size_t>> common_sides;
        for (size_t i = 0; i < k; ++i) {
            for (size_t j = 0; j < k; ++j) {
                if (equal((_points[(i + 1) % k] - _points[i]).length(), (copy._points[(j + 1) % k] - copy._points[j]).length())) {
                    common_sides.push_back(std::pair<size_t, size_t> {i, j});
                }
            }
        }

        bool flag = false;
        for (size_t x = 0; x < common_sides.size(); ++x) {
            size_t i = (common_sides[x].first + 1) % k;
            size_t j = (common_sides[x].second) % k;
            while ((i % k) != common_sides[x].first) {
                flag = true;

                if (!equal((_points[(i + 1) % k] - _points[i % k]).length(), (copy._points[mod((j - 1), k)] - copy._points[mod(j, k)]).length())) {
                    flag = false;
                    break;
                }
                ++i, --j;
            }

            if (flag) { return true; }
        }

        flag = false;
        for (size_t x = 0; x < common_sides.size(); ++x) {
            size_t i = (common_sides[x].first + 1) % k;
            size_t j = (common_sides[x].second + 1) % k;
            while ((i % k) != common_sides[x].first) {
                flag = true;
                if (!equal((_points[(i + 1) % k] - _points[i % k]).length(), (copy._points[(j + 1) % k] - copy._points[j % k]).length())) {
                    flag = false;
                    break;
                }
                ++i, ++j;
            }

            if (flag) { return true; }
        }



        return false;
    }

    bool operator==(const circle& another) const { return false; }
    bool operator!=(const circle& another) const { return true; }

    bool operator==(const shape& another) const override {
        const polygon& copy = dynamic_cast<const polygon&>(another);
        if (_points.size() != copy._points.size()) { return false; }

        for (auto & point : _points) {
            if (std::find(copy._points.begin(), copy._points.end(), point) == copy._points.end()) {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const shape& another) const override { return !((*this) == another); }

    void rotate(double angle) override {
        vector C = center();
        for (auto & pt : _points) {
            vector dC = pt - C;
            pt.x = cos(angle) * dC.x - sin(angle) * dC.y + C.x;
            pt.y = sin(angle) * dC.x + cos(angle) * dC.y + C.y;
        }
    }

    void scale(double coefficient) override {
        vector C = center();
        translate(-C);
        for (auto & point : _points) {
            point.x *= coefficient;
            point.y *= coefficient;
        }
        translate(C);
    }

    void translate(vector transform) override {
        for (auto & point : _points) {
            point.x += transform.x;
            point.y += transform.y;
        }
    }

    size_t vertices_count() const { return _points.size(); }
    const std::vector<vector>& get_vertices() const { return _points; }

protected:
    std::vector<vector> _points;
};


class rectangle: public polygon {
public:
    rectangle(const vector& center, const double height, const double width): _center(center), _height(height), _width(width) {
        _points.push_back(vector(center.x + width / 2.0, center.y - height / 2.0));
        _points.push_back(vector(center.x + width / 2.0, center.y + height / 2.0));
        _points.push_back(vector(center.x - width / 2.0, center.y + height / 2.0));
        _points.push_back(vector(center.x - width / 2.0, center.y - height / 2.0));
    }

    vector center() const override { return _center; }

    double height() const { return _height; }
    double width() const { return _width; }

protected:
    vector _center;
    double _height;
    double _width;
};

class square: public rectangle {
public:
    square(const vector& center, const double side): rectangle(center, side, side), _center(center), _side(side) {}

    circle circumscribed_circle() const {
        return circle(center(), _side * std::sqrt(2.0) / 2.0);
    }

    circle inscribed_circle() const {
        return circle(center(), _side / 2.0);
    }

    double side() const { return _side; }

protected:
    vector _center;
    double _side;
};

class triangle: public polygon {
public:
    triangle(const vector& a, const vector& b, const vector& c): polygon(std::vector<vector> {a, b, c}) {}

    circle circumscribed_circle() const {
        vector A = _points[0];
        vector B = _points[1];
        vector C = _points[2];

        double a = (B - C).length();
        double b = (C - A).length();
        double c = (A - B).length();

        double Cx = (A.x * A.x + A.y * A.y) * (B.y - C.y);
        Cx += (B.x * B.x + B.y * B.y) * (C.y - A.y);
        Cx += (C.x * C.x + C.y * C.y) * (A.y - B.y);

        double Cy = (A.x * A.x + A.y * A.y) * (C.x - B.x);
        Cy += (B.x * B.x + B.y * B.y) * (A.x - C.x);
        Cy += (C.x * C.x + C.y * C.y) * (B.x - A.x);

        double D = 2 * (A.x * (B.y - C.y) + B.x * (C.y - A.y) + C.x * (A.y - B.y));

        return circle(vector(Cx, Cy) / D, a * b * c / (4 * area()));
    }

    circle inscribed_circle() const {
        vector A = _points[0];
        vector B = _points[1];
        vector C = _points[2];

        double a = (B - C).length();
        double b = (C - A).length();
        double c = (A - B).length();

        vector centroid = (a * A + b * B + c * C) / (a + b + c);

        double p = perimeter() / 2.0;
        return circle(centroid, std::sqrt( (p - a) * (p - b) * (p - c) / p) );
    }
};


#endif //CPP_GEOMETRY_H
