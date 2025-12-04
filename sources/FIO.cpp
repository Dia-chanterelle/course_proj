#include "../headers/FIO.h"

FIO::FIO(const std::string& ln, const std::string& fn, const std::string& pt)
    : lastName(ln), firstName(fn), patronymic(pt) {
}

std::ostream& operator<<(std::ostream& os, const FIO& fio) {
    os << fio.lastName << ' ' << fio.firstName << ' ' << fio.patronymic;
    return os;
}

std::istream& operator>>(std::istream& is, FIO& fio) {
    is >> fio.lastName >> fio.firstName >> fio.patronymic;
    return is;
}

bool FIO::operator==(const FIO& other) const {
    return lastName == other.lastName &&
        firstName == other.firstName &&
        patronymic == other.patronymic;
}

std::string FIO::getFullName() const {
    return lastName + " " + firstName + " " + patronymic;
}