#include "../headers/Reader.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstring>

Reader::Reader(const FIO& f, const std::string& log, const std::string& pwd,
    int age, const std::string& readerId)
    : Person(f, log, pwd, age, readerId) {
}

void Reader::showMenu() {
    std::cout << "=== Меню читателя ===\n";
    std::cout << "ID: " << id << ", Пользователь: " << fio.getFullName()
        << ", возраст: " << age << "\n";
    std::cout << "1. Просмотреть книги\n";
    std::cout << "2. Поиск книг\n";
    std::cout << "3. Мои книги\n";
    std::cout << "4. Изменить профиль\n";
    std::cout << "5. Выход\n";
}

void Reader::borrowBook(const std::string& bookId) {
    borrowedBooks.emplace_back(bookId, id, Date());
}

void Reader::returnBook(const std::string& bookId) {
    borrowedBooks.erase(
        std::remove_if(borrowedBooks.begin(), borrowedBooks.end(),
            [bookId](const BorrowedBook& bb) { return bb.getBookId() == bookId; }),
        borrowedBooks.end()
    );
}

void Reader::showBorrowedBooks() const {
    std::cout << "Книги, взятые читателем " << fio.getFullName()
        << " (ID: " << id << ", возраст: " << age << "):\n";
    for (const auto& book : borrowedBooks) {
        std::cout << "ID книги: " << book.getBookId() << std::endl;
    }
}

void Reader::saveToBinaryFile(std::ofstream& file) const {
    Person::saveToBinaryFile(file);

    size_t borrowedCount = borrowedBooks.size();
    file.write(reinterpret_cast<const char*>(&borrowedCount), sizeof(borrowedCount));

    for (const auto& book : borrowedBooks) {
        std::string bookId = book.getBookId();
        size_t bookIdSize = bookId.size();
        file.write(reinterpret_cast<const char*>(&bookIdSize), sizeof(bookIdSize));
        file.write(bookId.c_str(), bookIdSize);
    }
}

void Reader::loadFromBinaryFile(std::ifstream& file) {
    Person::loadFromBinaryFile(file);

    size_t borrowedCount;
    file.read(reinterpret_cast<char*>(&borrowedCount), sizeof(borrowedCount));

    borrowedBooks.clear();
    for (size_t i = 0; i < borrowedCount; ++i) {
        size_t bookIdSize;
        file.read(reinterpret_cast<char*>(&bookIdSize), sizeof(bookIdSize));
        char* bookIdBuffer = new char[bookIdSize + 1];
        file.read(bookIdBuffer, bookIdSize);
        bookIdBuffer[bookIdSize] = '\0';
        std::string bookId = bookIdBuffer;
        delete[] bookIdBuffer;

        borrowedBooks.emplace_back(bookId, id, Date());
    }
}

void Reader::saveAllToBinaryFile(const std::map<std::string, std::shared_ptr<Reader>>& readersMap,
    const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        size_t count = readersMap.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const auto& pair : readersMap) {
            pair.second->saveToBinaryFile(file);
        }
        file.close();
    }
}

std::map<std::string, std::shared_ptr<Reader>> Reader::loadAllFromBinaryFile(const std::string& filename) {
    std::map<std::string, std::shared_ptr<Reader>> readersMap;
    std::ifstream file(filename, std::ios::binary);

    if (file.is_open()) {
        size_t count;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));

        for (size_t i = 0; i < count; ++i) {
            auto reader = std::make_shared<Reader>();
            reader->loadFromBinaryFile(file);
            readersMap[reader->getId()] = reader;
        }
        file.close();
    }

    return readersMap;
}

std::string Reader::generateId() const {
    return "R" + std::to_string(person_count + 1);
}