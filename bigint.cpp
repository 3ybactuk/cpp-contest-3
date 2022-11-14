#include <iostream>
#include <vector>
#include <string>
#include <sstream>


class bigint {
public:
    bigint (const bigint& other) = default;
    bigint& operator= (const bigint& other) = default;
    ~bigint () = default;

    bigint (long long num = 0): _digits(), _is_negative(num < 0) {
        num = std::abs(num);

        do {
            _digits.push_back(num % 10);
        } while ((num /= 10) > 0);
    }

    explicit operator bool () const {
        return _digits.size() > 1 || _digits[0] != 0;
    }

    std::string to_string () const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }

    bigint& operator+= (const bigint& other) {
        if (this->_is_negative != other._is_negative) {
            *this -= (-other);
        } else {
            int rem = 0;

            for (size_t i = 0; (i < _digits.size()) || (i < other._digits.size()) || rem; ++i) {
                int digit_res = rem;

                if ((i < _digits.size()) && (i < other._digits.size())) {
                    digit_res += _digits[i] + other._digits[i];
                } else if ((i < _digits.size()) || (i < other._digits.size())) {
                    digit_res += (i >= _digits.size()) ? other._digits[i] : _digits[i];
                }

                if (i == _digits.size()) {
                    _digits.push_back(digit_res % 10);
                } else {
                    _digits[i] = digit_res % 10;
                }

                rem = digit_res >= 10;
            }
        }

        return *this;
    }

    bigint& operator-= (const bigint& other) {
        if (_is_negative != other._is_negative) {
            *this += (-other);
        } else {
            bigint a = *this;
            bigint b = other;
            // Remove sign to perform abs() check:
            a._is_negative = false;
            b._is_negative = false;
            if (a < b) {
                b = *this;
                a = other;
                a._is_negative = !_is_negative;
            } else {
                a._is_negative = other._is_negative;
            }

            int owe = 0;
            for (size_t i = 0; i < a._digits.size() || owe; ++i) {
                int digit_res = -owe + a._digits[i];

                if (i < b._digits.size()) {
                    digit_res -= b._digits[i];
                }

                owe = digit_res < 0;
                if (owe) {
                    digit_res += 10;
                }

                if (i >= a._digits.size()) {
                    a._digits.push_back(digit_res);
                } else {
                    a._digits[i] = digit_res;
                }
            }

            while (a._digits.back() == 0 && a._digits.size() != 1) {
                a._digits.pop_back();
            }

            *this = a;
        }

        return *this;
    }

    bigint& operator*= (const bigint& other) {
        std::vector<int> mults(_digits.size() + other._digits.size(), 0);
        _is_negative = (_is_negative != other._is_negative);

        for (size_t i = 0; i < other._digits.size(); ++i) {
            for (size_t j = 0; j < _digits.size(); ++j) {
                mults[i + j] += other._digits[i] * _digits[j];
            }
        }
        _digits.resize(_digits.size() + other._digits.size());
        int rem = 0;
        for (size_t i = 0; i < _digits.size(); ++i) {
            rem += mults[i];
            mults[i] = rem % 10;
            rem /= 10;
            _digits[i] = mults[i];
        }

        while (_digits.back() == 0 && _digits.size() != 1) {
            _digits.pop_back();
        }

        return *this;
    }

    bigint& operator++ () { return operator+=(1); }
    bigint& operator-- () { return operator-=(1); }

    const bigint operator++ (int) {
        bigint tmp = *this;
        operator+=(1);
        return tmp;
    }

    const bigint operator-- (int) {
        bigint tmp = *this;
        operator-=(1);
        return tmp;
    }

    bigint operator- () const {
        bigint tmp = *this;
        tmp._is_negative = !tmp._is_negative;
        return tmp;
    }

    bigint operator+ () const { return *this; }

    friend bool operator< (const bigint& lhs, const bigint& rhs);
    friend bool operator== (const bigint& lhs, const bigint& rhs);
    friend std::istream& operator>> (std::istream&, bigint&);
    friend std::ostream& operator<< (std::ostream&, const bigint&);
private:
    std::vector<int> _digits;
    bool _is_negative;
};

bigint operator+ (const bigint& first, const bigint& second) { return bigint(first) += second; }
bigint operator- (const bigint& first, const bigint& second) { return bigint(first) -= second; }
bigint operator* (const bigint& first, const bigint& second) { return bigint(first) *= second; }

bool operator< (const bigint& lhs, const bigint& rhs) {
    if (lhs._is_negative != rhs._is_negative) { return lhs._is_negative; }

    bool is_negative = lhs._is_negative;  // check if abs(lhs) < abs(rhs) or abs(lhs) > abs(rhs)
    if (lhs._digits.size() == rhs._digits.size()) {
        auto it_lhs = lhs._digits.rbegin();
        auto it_rhs = rhs._digits.rbegin();

        while (it_lhs != lhs._digits.rend()) {
            if (!is_negative && (*it_lhs < *it_rhs)) { return true; }
            else if (is_negative && (*it_lhs > *it_rhs)) { return true; }
            else if (*it_lhs != *it_rhs) { return false; }
            ++it_lhs;
            ++it_rhs;
        }
    }
    else if ((lhs._digits.size() < rhs._digits.size()) != is_negative) {
        return true;
    } else if (lhs._digits.size() != rhs._digits.size()) {
        return false;
    }

    return false;
}

bool operator== (const bigint& lhs, const bigint& rhs) {
    if ((lhs._is_negative != rhs._is_negative) || (lhs._digits.size() != rhs._digits.size())) {
        return false;
    } else {
        auto it_lhs = lhs._digits.begin();
        auto it_rhs = rhs._digits.begin();

        while (it_lhs != lhs._digits.end()) {
            if (*it_lhs != *it_rhs) { return false; }
            ++it_lhs;
            ++it_rhs;
        }
    }

    return true;
}

bool operator<= (const bigint& lhs, const bigint& rhs) { return (lhs < rhs) || (lhs == rhs); }
bool operator> (const bigint& lhs, const bigint& rhs) { return !(lhs <= rhs); }
bool operator>= (const bigint& lhs, const bigint& rhs) { return !(lhs < rhs); }
bool operator!= (const bigint& lhs, const bigint& rhs) { return !(lhs == rhs); }

std::istream& operator>> (std::istream& is, bigint& num) {
    std::string buff;
    is >> buff;

    if (buff[0] == '-') {
        buff[0] = '0';
        num._is_negative = true;
    }
    num._digits.resize(buff.size());
    for (int i = buff.size() - 1; i >= 0; --i) {
        num._digits[buff.size() - i - 1] = buff[i] - '0';
    }

    while (num._digits.back() == 0 && num._digits.size() != 1) {
        num._digits.pop_back();
    }

    return is;
}

std::ostream& operator<<(std::ostream& os, const bigint& num) {
    if (num._is_negative) { os << '-'; }
    for (auto it = num._digits.rbegin(); it != num._digits.rend(); ++it) {
        os << *it;
    }

    return os;
}

int main() {
//    bigint x;
//    std::cin >> x;
//    factorial(x);

//    bigint x, y;
//    char op;
//
//    std::cin >> x >> op >> y;
//
//    bigint tmp = x;
//    switch (op) {
//        case '-':
//            tmp -= y;
//            std::cout << "x -= y: " << tmp << std::endl;
//            std::cout << " x - y: " << (x - y) << std::endl;
//            break;
//        case '+':
//            tmp += y;
//            std::cout << "x += y: " << tmp << std::endl;
//            std::cout << " x + y: " << (x + y) << std::endl;
//            break;
//        case '*':
//            tmp *= y;
//            std::cout << "x *= y: " << tmp << std::endl;
//            std::cout << " x * y: " << (x * y) << std::endl;
//            break;
//        default:
//            break;
//    }

    return 0;
}
