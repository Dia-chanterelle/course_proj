#include "../headers/Librarian.h"
#include <iostream>
#include <fstream>
#include <cstring>

//Librarian::Librarian(const FIO& f, const std::string& log, const std::string& pwd,
//    int age, const std::string& librarianId, const std::string& dept)
//    : Person(f, log, pwd, age, librarianId), department(dept) {
//}

Librarian::Librarian(const FIO& f, const std::string& log, const std::string& pwd,
    int age, const std::string& librarianId,
    const std::string& dept, bool isEncrypted)
    : Person(f, log, pwd, age, librarianId, isEncrypted), department(dept) {
}



void Librarian::showMenu() {
    std::cout << "=== Меню библиотекаря ===\n";
    std::cout << "1. Работа с книгами\n";
    std::cout << "   |-- Все книги\n";
    std::cout << "   |-- Поиск книг\n";
    std::cout << "   |-- Фильтрация книг\n";
    std::cout << "   |-- Просмотр авторов\n";
    std::cout << "   |-- Поиск авторов\n";
    std::cout << "   |-- Просмотр книг по автору\n";
    std::cout << "2. Редактирование книг\n";
    std::cout << "   |-- Добавить книгу\n";
    std::cout << "   |-- Редактировать книгу\n";
    std::cout << "   |-- Удалить книгу\n";
    std::cout << "3. Работа с читателями\n";
    std::cout << "   |-- Просмотреть всех читателей\n";
    std::cout << "   |-- Просмотреть книги конкретного читателя\n";
    std::cout << "   |-- Зарегистрировать нового читателя\n";
    std::cout << "4. Выдача/приём книг\n";
    std::cout << "   |-- Выдать книгу\n";
    std::cout << "   |-- Принять книгу\n";
    std::cout << "5. Выход\n";
}


void Librarian::saveToBinaryFile(std::ofstream& file) const {
    Person::saveToBinaryFile(file);

    size_t deptSize = department.size();
    file.write(reinterpret_cast<const char*>(&deptSize), sizeof(deptSize));
    file.write(department.c_str(), deptSize);
}

void Librarian::loadFromBinaryFile(std::ifstream& file) {
    Person::loadFromBinaryFile(file);

    size_t deptSize;
    file.read(reinterpret_cast<char*>(&deptSize), sizeof(deptSize));
    char* deptBuffer = new char[deptSize + 1];
    file.read(deptBuffer, deptSize);
    deptBuffer[deptSize] = '\0';
    department = deptBuffer;
    delete[] deptBuffer;
}

void Librarian::saveAllToBinaryFile(const std::map<std::string, std::shared_ptr<Librarian>>& librariansMap,
    const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        size_t count = librariansMap.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const auto& pair : librariansMap) {
            pair.second->saveToBinaryFile(file);
        }
        file.close();
    }
}

std::map<std::string, std::shared_ptr<Librarian>> Librarian::loadAllFromBinaryFile(const std::string& filename) {
    std::map<std::string, std::shared_ptr<Librarian>> librariansMap;
    std::ifstream file(filename, std::ios::binary);

    if (file.is_open()) {
        size_t count;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));

        for (size_t i = 0; i < count; ++i) {
            auto librarian = std::make_shared<Librarian>();
            librarian->loadFromBinaryFile(file);
            librariansMap[librarian->getId()] = librarian;
        }
        file.close();
    }

    return librariansMap;
}

//std::string Librarian::generateId() const {
//    return "L" + std::to_string(person_count + 1);
//}