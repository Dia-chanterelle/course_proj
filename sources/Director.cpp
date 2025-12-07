#include "../headers/Director.h"
#include <iostream>
#include <fstream>
#include <cstring>

Director::Director(const FIO& f, const std::string& log, const std::string& pwd,
    int age, const std::string& directorId)
    : Librarian(f, log, pwd, age, directorId, "Administration") {
}

void Director::showMenu() {
    std::cout << "=== Меню директора ===\n";
    std::cout << "1. Просмотр книг\n";
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
    std::cout << "3. Работа с пользователями\n";
    std::cout << "   |-- Просмотреть всех пользователей\n";
    std::cout << "   |-- Зарегистрировать библиотекаря\n";
    std::cout << "   |-- Зарегистрировать директора\n";
    std::cout << "   |-- Удалить пользователя\n";
    std::cout << "4. Выдача/приём книг\n";
    std::cout << "   |-- Выдать книгу\n";
    std::cout << "   |-- Принять книгу\n";
    std::cout << "5. Статистика и отчёты\n";
    std::cout << "6. История действий\n";
    std::cout << "7. Выход\n";
}



void Director::saveAllToBinaryFile(const std::map<std::string, std::shared_ptr<Director>>& directorsMap,
    const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        size_t count = directorsMap.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const auto& pair : directorsMap) {
            pair.second->saveToBinaryFile(file);
        }
        file.close();
    }
}

std::map<std::string, std::shared_ptr<Director>> Director::loadAllFromBinaryFile(const std::string& filename) {
    std::map<std::string, std::shared_ptr<Director>> directorsMap;
    std::ifstream file(filename, std::ios::binary);

    if (file.is_open()) {
        size_t count;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));

        for (size_t i = 0; i < count; ++i) {
            auto director = std::make_shared<Director>();
            director->loadFromBinaryFile(file);
            directorsMap[director->getId()] = director;
        }
        file.close();
    }

    return directorsMap;
}

std::string Director::generateId() const {
    return "D" + std::to_string(person_count + 1);
}