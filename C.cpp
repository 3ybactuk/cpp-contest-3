#include <iostream>
#include <algorithm>    // for std::min and std::max
#include <stdexcept>    // for std::out_of_range
#include <string>       // for std::to_string
#include <array>

template<typename vt, size_t height, size_t width>
class matrix {
public:
    typedef matrix<vt, std::max<size_t>(height - 1, 1), std::max<size_t>(width - 1, 1)> matrix_minor;

    matrix(const vt& num = 0) {
        _matrix = new vt[height * width];
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                _matrix[i * width + j] = num;
            }
        }
    }

    matrix(const matrix& other) {
        _matrix = new vt[height * width];
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                _matrix[i * width + j] = other(i, j);
            }
        }
    }

    matrix& operator= (const matrix& other) {
        if (this != &other) {
            this->~matrix();
            new (this) matrix(other);
        }

        return *this;
    }

    ~matrix() { delete[] _matrix; }


    void print() {
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                std::cout << _matrix[i * width + j] << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    const vt& operator() (const size_t& i, const size_t& j) const { return _matrix[i * width + j]; }
    vt& operator() (const size_t& i, const size_t& j) { return _matrix[i * width + j]; }

    vt& at(const size_t& i, const size_t& j) { return _matrix[i * width + j]; }
    const vt& at(const size_t& i, const size_t& j) const { return _matrix[i * width + j]; }

    matrix<vt, width, height> transposed() const {
        matrix<vt, width, height> matrix_T;

        for (size_t j = 0; j < width; ++j) {
            for (size_t i = 0; i < height; ++i) {
                matrix_T(j, i) = _matrix[i * width + j];
            }
        }

        return matrix_T;
    }

    vt trace() const {
        vt sum = 0;
        for (size_t i = 0; i < height; ++i) {
            sum += _matrix[i * width + i];
        }

        return sum;
    }

    matrix_minor minor(size_t i, size_t j) {
        matrix_minor res;

        size_t a = 0, b = 0;
        for (size_t x = 0; x < height; ++x) {
            if (x != i) {
                for (size_t y = 0; y < width; ++y) {
                    if (y != j) {
                        res(a, b) = _matrix[x * width + y];
                        ++b;
                    }
                }
                ++a;
            }
        }

        return res;
    }

    vt det() const {
        return 0;
    }

    template<typename vt2, size_t x, size_t y>
    matrix& operator+= (const matrix<vt2, x, y>& other) {
        if (height != x || width != y) {
            std::cout << ("Error: Cannot perform operation (+ or -), sizes are different!") << std::endl;
            exit(-1);
        }

        for (size_t i = 0; i < x; ++i) {
            for (size_t j = 0; j < y; ++j) {
                _matrix[i * width + j] += other(i, j);
            }
        }

        return *this;
    }

    template<typename vt2>
    matrix& operator+= (const vt2 num) { return *this += matrix<vt2, height, width>(num); }

    template<typename vt2, size_t x, size_t y>
    matrix& operator-= (const matrix<vt2, x, y>& other) { return *this += -(other); }

    template<typename vt2>
    matrix& operator-= (const vt2 num) { return *this -= matrix<vt2, height, width>(num); }

    template<typename vt2, size_t x, size_t y>
    matrix& operator*= (const matrix<vt2, x, y>& other) {
        matrix<vt, height, width> tmp(*this);

        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < y; ++j) {
                vt sum = 0;
                for (size_t k = 0; k < width; ++k) {
                    sum += _matrix[i * width + k] * other(k, j);
                }
                tmp(i, j) = sum;
            }
        }

        *this = tmp;
        return *this;
    }

    template<typename vt2>
    matrix& operator*= (const vt2 num) {
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                _matrix[i * width + j] *= num;
            }
        }

        return *this;
    }


    const matrix operator- () const {
        matrix tmp = *this;
        for (size_t i = 0; i < height; ++i) {
            for (size_t j = 0; j < width; ++j) {
                tmp(i, j) = -tmp(i, j);
            }
        }

        return tmp;
    }

    const matrix operator+ () const { return *this; }

private:
    vt * _matrix;
};

template<typename vt, size_t height, size_t width, typename vt2, size_t x, size_t y>
bool operator== (const matrix<vt, height, width>& lhs, const matrix<vt2, x, y>& rhs) {
    if (x != height || y != width) { return false; }

    for (size_t i = 0; i < x; ++i) {
        for (size_t j = 0; j < y; ++j) {
            if (lhs(i, j) != rhs(i, j)) { return false; }
        }
    }

    return true;
}

template<typename vt, size_t height, size_t width, typename vt2, size_t x, size_t y>
bool operator!= (const matrix<vt, height, width>& lhs, const matrix<vt2, x, y>& rhs) { return !(lhs == rhs); }

template<typename vt, size_t height, size_t width, typename vt2, size_t x, size_t y>
matrix<vt, height, width> operator+ (const matrix<vt, height, width>& first, const matrix<vt2, x, y>& second) {
    return matrix<vt, height, width>(first) += second;
}

template<typename vt, size_t height, size_t width, typename vt2>
matrix<vt, height, width> operator+ (const matrix<vt, height, width>& first, const vt2 num) {
    return matrix<vt, height, width>(first) += num;
}

template<typename vt, size_t height, size_t width, typename vt2>
matrix<vt, height, width> operator+ (const vt2 num, const matrix<vt, height, width>& first) {
    return matrix<vt, height, width>(first) += num;
}

template<typename vt, size_t height, size_t width, typename vt2, size_t x, size_t y>
matrix<vt, height, width> operator- (const matrix<vt, height, width>& first, const matrix<vt2, x, y>& second) {
    return matrix<vt, height, width>(first) -= second;
}

template<typename vt, size_t height, size_t width, typename vt2>
matrix<vt, height, width> operator- (const matrix<vt, height, width>& first, const vt2 num) {
    return matrix<vt, height, width>(first) -= num;
}

template<typename vt, size_t height, size_t width, typename vt2>
matrix<vt, height, width> operator- (const vt2 num, const matrix<vt, height, width>& second) {
    return matrix<vt, height, width>(num) -= second;
}

template<typename vt, size_t height, size_t width, typename vt2, size_t y>
matrix<vt, height, y> operator* (const matrix<vt, height, width>& first, const matrix<vt2, width, y>& second) {
    matrix<vt, height, y> res;
    for (size_t i = 0; i < height; ++i) {
        for (size_t j = 0; j < y; ++j) {
            for (size_t k = 0; k < width; ++k) {
                res(i, j) += first(i, k) * second(k, j);
            }
        }
    }

    return res;
}

template<typename vt, size_t height, size_t width, typename vt2>
matrix<vt, height, width> operator* (const matrix<vt, height, width>& first, const vt2 num) {
    return matrix<vt, height, width>(first) *= num;
}

template<typename vt, size_t height, size_t width, typename vt2>
matrix<vt, height, width> operator* (const vt2 num, const matrix<vt, height, width>& first) {
    return matrix<vt, height, width>(first) *= num;
}


int main() {
    matrix<int, 6, 6> A;
    matrix<int, 6, 6> B;

    // {{0, 4, 0, 0, 0, 0}, {0, 0, 0, 50, 0, -21}, {86, 10, 0, -26, 0, 0}, {0, 94, 0, 0, 0, 0}, {-52, 6, 0, 0, -6, 0}, {0, 0, 0, -75, 0, 71}})
    // matrix<i, 6, 6>({{0, -77, 0, 57, -100, 0}, {-98, 0, 0, 17, 22, 81}, {0, 0, -23, -50, -85, -5}, {47, 18, 9, 36, -33, 0}, {0, -46, 42, 0, -98, 0}, {0, 54, 79, 0, 0, 0}}

    std::array<std::array<int, 6>, 6> Arr { {{0, 4, 0, 0, 0, 0}, {0, 0, 0, 50, 0, -21}, {86, 10, 0, -26, 0, 0}, {0, 94, 0, 0, 0, 0}, {-52, 6, 0, 0, -6, 0}, {0, 0, 0, -75, 0, 71}}};
    std::array<std::array<int, 6>, 6> Brr { {{0, -77, 0, 57, -100, 0}, {-98, 0, 0, 17, 22, 81}, {0, 0, -23, -50, -85, -5}, {47, 18, 9, 36, -33, 0}, {0, -46, 42, 0, -98, 0}, {0, 54, 79, 0, 0, 0}} };

    for (size_t i = 0; i < 6; ++i) {
        for (size_t j = 0; j < 6; ++j) {
            A(i, j) = i + j;//Arr[i][j];
            B(i, j) = i + j;//Brr[i][j];
        }
    }
    A.print();

    A.minor(2, 2).print();

    return 0;
}