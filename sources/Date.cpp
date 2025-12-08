#include "../headers/Date.h"
#include <chrono>
#include <ctime>

std::istream& operator>>(std::istream& in, Date& date) {
    char dot;
    in >> date.day >> dot >> date.month >> dot >> date.year;
    return in;
}

std::ostream& operator<<(std::ostream& out, const Date& date) {
    out << (date.day < 10 ? "0" : "") << date.day << "."
        << (date.month < 10 ? "0" : "") << date.month << "."
        << date.year;
    return out;
}

bool Date::operator==(const Date& other) const {
    return day == other.day && month == other.month && year == other.year;
}

bool Date::isValid(int d, int m, int y) {
    if (y < 1 || m < 1 || m > 12 || d < 1) return false;
    static int daysInMonth[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    int maxDay = daysInMonth[m - 1];
    if (m == 2 && ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0))) maxDay = 29;
    return d <= maxDay;
}

Date Date::today() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
    localtime_s(&tm, &t);
    return Date(tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

Date Date::addDays(int days) const {
    std::tm tm{};
    tm.tm_mday = day;
    tm.tm_mon = month - 1;
    tm.tm_year = year - 1900;

    std::time_t t = std::mktime(&tm);
    t += days * 24 * 60 * 60;

    std::tm newTm{};
    localtime_s(&newTm, &t);

    return Date(newTm.tm_mday, newTm.tm_mon + 1, newTm.tm_year + 1900);
}
