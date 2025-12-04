#pragma once
#include <iostream>

class Date {
private:
    int day, month, year;
public:
    Date(int d, int m, int y);
    Date();

    friend std::ostream& operator<<(std::ostream& os, const Date& dt);
    friend std::istream& operator>>(std::istream& is, Date& dt);
    bool operator==(const Date& other) const;

    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }

    static bool isValid(int d, int m, int y);
};