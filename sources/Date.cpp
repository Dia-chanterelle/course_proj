#include "../headers/Date.h"
#include <stdexcept>

Date::Date() {
	std::cout << "Введите дату" << std::endl;
	while (true) {
		std::cout << "день: ";
		std::cin >> day;
		if (std::cin.good()) {
			if (day < 1 or day>31) {
				std::cout << "Введите число от 1 до 31" << std::endl;
				continue;
			}
			else break;
		}
		std::cin.clear();
		std::cout << "Неверный формат данных"<<std::endl;
		std::cin.ignore(10, '\n');
	}
	while (true) {
		std::cout << "месяц: ";
		std::cin >> month;
		if (std::cin.good()) {
			if (month < 1 or month>12) {
				std::cout << "Введите число от 1 до 12" << std::endl;
				continue;
			}
			else break;
		}
		std::cin.clear();
		std::cout << "Неверный формат данных" << std::endl;
		std::cin.ignore(10, '\n');
	}
	while (true) {
		std::cout << "год: ";
		std::cin >> year;
		if (std::cin.good()) {
			if (year < 2000 or year>2100) {
				std::cout << "Введите число от 2000 до 2100" << std::endl;
				continue;
			}
			else break;
		}
		std::cin.clear();
		std::cout << "Неверный формат данных" << std::endl;
		std::cin.ignore(10, '\n');
	}
}

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

bool Date::operator==(const Date& other) const {
	return day == other.day && month == other.month && year == other.year;
}