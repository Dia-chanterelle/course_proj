#include "../headers/PersonFactory.h"
#include <fstream>
#include <cstring>

std::shared_ptr<Person> PersonFactory::createPersonFromBinaryFile(std::ifstream& file) {
    std::streampos startPos = file.tellg();

    size_t idSize;
    file.read(reinterpret_cast<char*>(&idSize), sizeof(idSize));
    file.seekg(idSize, std::ios::cur);

    size_t loginSize;
    file.read(reinterpret_cast<char*>(&loginSize), sizeof(loginSize));
    file.seekg(loginSize, std::ios::cur);

    size_t pwdSize;
    file.read(reinterpret_cast<char*>(&pwdSize), sizeof(pwdSize));
    file.seekg(pwdSize, std::ios::cur);

    file.seekg(sizeof(int), std::ios::cur);

    size_t nameSize;
    file.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
    file.seekg(nameSize, std::ios::cur);

    size_t roleSize;
    file.read(reinterpret_cast<char*>(&roleSize), sizeof(roleSize));
    char* roleBuffer = new char[roleSize + 1];
    file.read(roleBuffer, roleSize);
    roleBuffer[roleSize] = '\0';
    std::string role = roleBuffer;
    delete[] roleBuffer;

    file.seekg(startPos);

    std::shared_ptr<Person> person;
    if (role == "Reader") {
        person = std::make_shared<Reader>();
    }
    else if (role == "Librarian") {
        person = std::make_shared<Librarian>();
    }
    else if (role == "Director") {
        person = std::make_shared<Director>();
    }
    else {
        std::cerr << "Неизвестный тип пользователя: " << role << std::endl;
        return nullptr;
    }

    if (person) {
        person->loadFromBinaryFile(file);
    }

    return person;
}

std::map<std::string, std::shared_ptr<Person>> PersonFactory::loadAllFromBinaryFile(const std::string& filename) {
    std::map<std::string, std::shared_ptr<Person>> personsMap;
    std::ifstream file(filename, std::ios::binary);

    if (file.is_open()) {
        size_t count;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));

        for (size_t i = 0; i < count; ++i) {
            auto person = createPersonFromBinaryFile(file);
            if (person) {
                personsMap[person->getId()] = person;
            }
        }
        file.close();
    }
    else {
        std::cerr << "Не удалось открыть файл: " << filename << std::endl;
    }

    return personsMap;
}

void PersonFactory::saveAllToBinaryFile(const std::map<std::string, std::shared_ptr<Person>>& personsMap,
    const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        size_t count = personsMap.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const auto& pair : personsMap) {
            pair.second->saveToBinaryFile(file);
        }
        file.close();
    }
    else {
        std::cerr << "Не удалось создать файл: " << filename << std::endl;
    }
}