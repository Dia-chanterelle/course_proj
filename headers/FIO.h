#pragma once
#include <iostream>

class FIO {
private:
    std::string lastName, firstName, patronymic;
public:
    FIO(const std::string& ln = "", const std::string& fn = "", const std::string& pt = "");

    friend std::ostream& operator<<(std::ostream& os, const FIO& fio);
    friend std::istream& operator>>(std::istream& is, FIO& fio);
    bool operator==(const FIO& other) const;

    std::string getFullName() const;
};