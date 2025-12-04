#pragma once
#include "FIO.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>

class Author {
private:
    FIO fio;
    std::string biography;
    std::string id;
    std::vector<std::string> bookIds;
    static int author_count;

public:
    Author(const FIO& f = FIO(), const std::string& bio = "",
        const std::string& authorId = "");

    friend std::ostream& operator<<(std::ostream& os, const Author& author);
    bool operator==(const Author& other) const;

    // Геттеры
    FIO getFIO() const { return fio; }
    std::string getId() const { return id; }
    std::string getBiography() const { return biography; }
    std::vector<std::string> getBookIds() const { return bookIds; }

    // Методы работы с книгами
    void addBook(const std::string& bookId);
    void removeBook(const std::string& bookId);
    bool hasBook(const std::string& bookId) const;

    // Статические методы
    static int getAuthorCount() { return author_count; }
    static void setAuthorCount(int count) { author_count = count; }

    // Методы для работы с Repository
    void saveToBinaryFile(std::ofstream& file) const;
    void loadFromBinaryFile(std::ifstream& file);

private:
    std::string generateId() const;
};