#include "../headers/Person.h"
#include "../headers/SimpleCrypto.h"
#include <string>
#include <fstream>
#include <cstring>

int Person::person_count = 0;

Person::Person(const FIO& f, const std::string& log, const std::string& pwd,
    int age, const std::string& personId)
    : fio(f), login(log), age(age) {
    encryptedPassword = SimpleCrypto::simpleEncrypt(pwd);

    if (personId.empty()) {
        id = generateId();
        person_count++;
    }
    else {
        id = personId;
        try {
            std::string numStr;
            for (size_t i = 1; i < personId.length(); i++) {
                if (isdigit(personId[i])) {
                    numStr += personId[i];
                }
            }
            if (!numStr.empty()) {
                int num = std::stoi(numStr);
                if (num >= person_count) {
                    person_count = num + 1;
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
    }
}

bool Person::authenticate(const std::string& pwd) const {
    return SimpleCrypto::compareEncrypted(pwd, encryptedPassword);
}

void Person::changePassword(const std::string& newPwd) {
    encryptedPassword = SimpleCrypto::simpleEncrypt(newPwd);
}

std::ostream& operator<<(std::ostream& os, const Person& person) {
    os << "ID: " << person.id << "\n";
    os << "Логин: " << person.login << "\n";
    os << "ФИО: " << person.fio << "\n";
    os << "Возраст: " << person.age << "\n";
    os << "Роль: " << person.getRole() << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, Person& person) {
    std::cout << "Введите логин: ";
    is >> person.login;

    std::cout << "Введите пароль: ";
    std::string password;
    is >> password;
    person.encryptedPassword = SimpleCrypto::simpleEncrypt(password);

    std::cout << "Введите возраст: ";
    is >> person.age;

    std::cout << "Введите ФИО (фамилия имя отчество): ";
    is >> person.fio;

    return is;
}

void Person::saveToBinaryFile(std::ofstream& file) const {
    size_t idSize = id.size();
    file.write(reinterpret_cast<const char*>(&idSize), sizeof(idSize));
    file.write(id.c_str(), idSize);

    size_t loginSize = login.size();
    file.write(reinterpret_cast<const char*>(&loginSize), sizeof(loginSize));
    file.write(login.c_str(), loginSize);

    size_t pwdSize = encryptedPassword.size();
    file.write(reinterpret_cast<const char*>(&pwdSize), sizeof(pwdSize));
    file.write(encryptedPassword.c_str(), pwdSize);

    file.write(reinterpret_cast<const char*>(&age), sizeof(age));

    std::string fullName = fio.getFullName();
    size_t nameSize = fullName.size();
    file.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
    file.write(fullName.c_str(), nameSize);

    std::string role = getRole();
    size_t roleSize = role.size();
    file.write(reinterpret_cast<const char*>(&roleSize), sizeof(roleSize));
    file.write(role.c_str(), roleSize);
}

void Person::loadFromBinaryFile(std::ifstream& file) {
    size_t idSize;
    file.read(reinterpret_cast<char*>(&idSize), sizeof(idSize));
    char* idBuffer = new char[idSize + 1];
    file.read(idBuffer, idSize);
    idBuffer[idSize] = '\0';
    id = idBuffer;
    delete[] idBuffer;

    size_t loginSize;
    file.read(reinterpret_cast<char*>(&loginSize), sizeof(loginSize));
    char* loginBuffer = new char[loginSize + 1];
    file.read(loginBuffer, loginSize);
    loginBuffer[loginSize] = '\0';
    login = loginBuffer;
    delete[] loginBuffer;

    size_t pwdSize;
    file.read(reinterpret_cast<char*>(&pwdSize), sizeof(pwdSize));
    char* pwdBuffer = new char[pwdSize + 1];
    file.read(pwdBuffer, pwdSize);
    pwdBuffer[pwdSize] = '\0';
    encryptedPassword = pwdBuffer;
    delete[] pwdBuffer;

    file.read(reinterpret_cast<char*>(&age), sizeof(age));

    size_t nameSize;
    file.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
    char* nameBuffer = new char[nameSize + 1];
    file.read(nameBuffer, nameSize);
    nameBuffer[nameSize] = '\0';
    std::string fullName = nameBuffer;
    delete[] nameBuffer;

    size_t space1 = fullName.find(' ');
    size_t space2 = fullName.find(' ', space1 + 1);
    if (space1 != std::string::npos && space2 != std::string::npos) {
        std::string lastName = fullName.substr(0, space1);
        std::string firstName = fullName.substr(space1 + 1, space2 - space1 - 1);
        std::string patronymic = fullName.substr(space2 + 1);
        fio = FIO(lastName, firstName, patronymic);
    }

    size_t roleSize;
    file.read(reinterpret_cast<char*>(&roleSize), sizeof(roleSize));
    char* roleBuffer = new char[roleSize + 1];
    file.read(roleBuffer, roleSize);
    roleBuffer[roleSize] = '\0';
    delete[] roleBuffer;
}

std::string Person::generateId() const {
    return "P" + std::to_string(person_count + 1);
}