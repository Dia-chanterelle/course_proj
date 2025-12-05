#include "../headers/Date.h"
#include <stdexcept>

Date::Date() : day(1), month(1), year(2000) {}

Date::Date(int d, int m, int y) : day(d), month(m), year(y) {
    if (!isValid(d, m, y)) throw std::invalid_argument("Invalid date");
}

bool Date::isValid(int d, int m, int y) {
    if (y < 1900 || y > 2100) return false;
    if (m < 1 || m > 12) return false;
    if (d < 1) return false;

    int daysInMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (m == 2 && (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0))) {
        return d <= 29;
    }
    return d <= daysInMonth[m - 1];
}

std::ostream& operator<<(std::ostream& os, const Date& dt) {
    os << dt.day << '.' << dt.month << '.' << dt.year;
    return os;
}

std::istream& operator>>(std::istream& is, Date& dt) {
    char dot;
    is >> dt.day >> dot >> dt.month >> dot >> dt.year;
    if (!Date::isValid(dt.day, dt.month, dt.year)) {
        is.setstate(std::ios::failbit);
    }
    return is;
}
