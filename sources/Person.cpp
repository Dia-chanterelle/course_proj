#include "../headers/Person.h"
#include "../headers/SimpleCrypto.h"
#include <string>
#include <fstream>
#include <cstring>

int Person::person_count = 0;

//Person::Person(const FIO& f, const std::string& log, const std::string& pwd,
//    int age, const std::string& personId)
//    : fio(f), login(log), age(age), id(personId) {
//    encryptedPassword = SimpleCrypto::simpleEncrypt(pwd);
//}

Person::Person(const FIO& f, const std::string& log, const std::string& pwd,
    int age, const std::string& personId, bool isEncrypted)
    : fio(f), login(log), age(age), id(personId) {
    if (isEncrypted)
        encryptedPassword = pwd; // уже зашифрованный пароль
    else
        encryptedPassword = SimpleCrypto::simpleEncrypt(pwd); // новый пароль
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
    std::string idStr(idSize, '\0');
    file.read(idStr.data(), idSize);
    id = std::move(idStr);

    size_t loginSize;
    file.read(reinterpret_cast<char*>(&loginSize), sizeof(loginSize));
    std::string loginStr(loginSize, '\0');
    file.read(loginStr.data(), loginSize);
    login = std::move(loginStr);

    size_t pwdSize;
    file.read(reinterpret_cast<char*>(&pwdSize), sizeof(pwdSize));
    std::string pwdStr(pwdSize, '\0');
    file.read(pwdStr.data(), pwdSize);
    encryptedPassword = std::move(pwdStr);

    file.read(reinterpret_cast<char*>(&age), sizeof(age));

    size_t nameSize;
    file.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
    std::string fullName(nameSize, '\0');
    file.read(fullName.data(), nameSize);

    size_t space1 = fullName.find(' ');
    size_t space2 = fullName.find(' ', space1 + 1);
    if (space1 != std::string::npos && space2 != std::string::npos) {
        fio = FIO(fullName.substr(0, space1),
            fullName.substr(space1 + 1, space2 - space1 - 1),
            fullName.substr(space2 + 1));
    }

    size_t roleSize;
    file.read(reinterpret_cast<char*>(&roleSize), sizeof(roleSize));
    std::string role(roleSize, '\0');
    file.read(role.data(), roleSize);
}

//std::string Person::generateId() const {
//    return "P" + std::to_string(person_count + 1);
//}