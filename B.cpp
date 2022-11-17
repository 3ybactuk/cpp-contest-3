#include <iostream>

template<size_t length>
class permutation {
public:
    permutation() {
        _perm = new unsigned[length];
        for (size_t i = 0; i < length; ++i) {
            _perm[i] = i;
        }
    }

    explicit permutation(const unsigned* array) {
        _perm = new unsigned[length];
        for (size_t i = 0; i < length; ++i) {
            _perm[i] = array[i];
        }
    }

    permutation(const permutation& other) {
        _perm = new unsigned[length];
        for (size_t i = 0; i < length; ++i) {
            _perm[i] = other[i];
        }
    }

    ~permutation() { delete _perm; }

    permutation& operator= (const permutation& other) {
        if (this != &other) {
            for (size_t i = 0; i < length; ++i) {
                _perm[i] = other[i];
            }
        }

        return *this;
    }

    unsigned& operator[] (size_t i) { return _perm[i]; }
    const unsigned& operator[] (size_t i) const { return _perm[i]; }

    permutation& operator*= (const permutation& other) {
        permutation<length> res;

        for (size_t i = 0; i < length; ++i) {
            res[i] = _perm[other[i]];
        }

        *this = res;
        return *this;
    }

    permutation& operator++ () { return *this = (*this).next(); }
    permutation& operator-- () { return *this = (*this).prev(); }

    const permutation operator++ (int) {
        permutation tmp = *this;
        *this = (*this).next();
        return tmp;
    }

    const permutation operator-- (int) {
        permutation tmp = *this;
        *this = (*this).prev();
        return tmp;
    }

    void operator() (unsigned* values) const {
        unsigned* val_cp = new unsigned[length];

        for (size_t i = 0; i < length; ++i) {
            val_cp[i] = values[i];
        }

        for (size_t i = 0; i < length; ++i) {
            values[_perm[i]] = val_cp[i];
        }
    }

    permutation next() const {
        permutation<length> res = *this;

        int point1 = length - 1;
        while (point1 > 0 && res[point1 - 1] >= res[point1]) { --point1; }

        if (point1 == 0) { return permutation<length>(); }

        int point2 = length - 1;
        while (res[point2] <= res[point1 - 1]) { point2--; }

        // pivots are on indexes: (point1 - 1) and (point2)
        std::swap(res[point1 - 1], res[point2]);

        for (int i = length - 1; i > point1; i--, point1++) {
            std::swap(res[point1], res[i]);
        }

        return res;
    }

    permutation inverse() const {
        permutation<length> res = *this;

        for (unsigned i = 0; i < length; ++i) {
            res[_perm[i]] = i;
        }

        return res;
    }

    permutation prev() const {
        permutation<length> res = *this;

        int point1 = length - 1;
        while (point1 > 0 && res[point1 - 1] <= res[point1]) { --point1; }

        if (point1 == 0) {
            for (int i = length - 1; i >= 0; --i) {
                res[length - i - 1] = i;
            }
            return res;
        }

        int point2 = length - 1;
        while (res[point2] >= res[point1 - 1]) { point2--; }

        // pivots are on indexes: (point1 - 1) and (point2)
        std::swap(res[point1 - 1], res[point2]);

        for (int i = length - 1; i > point1; i--, point1++) {
            std::swap(res[point1], res[i]);
        }

        return res;
    }

private:
    unsigned* _perm;
};

template<size_t length1, size_t length2>
bool operator<(const permutation<length1> &lhs, const permutation<length2> &rhs) {
    for (size_t i = 0; (i < length1) && (i < length2); ++i) {
        if (lhs[i] > rhs[i]) { return false; }
        else if (lhs[i] < rhs[i]) { return true; }
    }

    return length1 < length2;
}

template<size_t length1, size_t length2>
bool operator==(const permutation<length1> &lhs, const permutation<length2> &rhs) {
    for (size_t i = 0; (i < length1) && (i < length2); ++i) {
        if (lhs[i] != rhs[i]) { return false; }
    }

    return length1 == length2;
}

template<size_t length1, size_t length2>
bool operator<= (const permutation<length1> &lhs, const permutation<length2> &rhs) { return (lhs < rhs) || (lhs == rhs); }

template<size_t length1, size_t length2>
bool operator> (const permutation<length1> &lhs, const permutation<length2> &rhs) { return !(lhs <= rhs); }

template<size_t length1, size_t length2>
bool operator>= (const permutation<length1> &lhs, const permutation<length2> &rhs) { return !(lhs < rhs); }

template<size_t length1, size_t length2>
bool operator!= (const permutation<length1> &lhs, const permutation<length2> &rhs) { return !(lhs == rhs); }

template<size_t length>
permutation<length> operator* (const permutation<length>& first, const permutation<length>& second) { return permutation<length>(first) *= second; }


int main() {
    permutation<3> A;

    for (int i = 0; i < 3; ++i) {
        std::cout << A[i] << " ";
    }
    std::cout << std::endl;

    --A;

    for (int i = 0; i < 3; ++i) {
        std::cout << A[i] << " ";
    }

    return 0;
}