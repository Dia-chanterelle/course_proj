#pragma once
#include "Author.h"
#include <string>
#include <memory>
#include <fstream>
#include <map>

class Book {
private:
    std::string id;
    std::string title;
    std::shared_ptr<Author> author;
    std::string isbn;
    int year;
    int quantity;
    std::string genre;

public:
    Book(const std::string& t = "", std::shared_ptr<Author> a = nullptr,
        const std::string& i = "", int y = 0, int q = 1,
        const std::string& g = "", const std::string& bookId = "");

    friend std::ostream& operator<<(std::ostream& os, const Book& book);
    bool operator==(const Book& other) const;

    std::string getId() const { return id; }
    std::string getTitle() const { return title; }
    std::shared_ptr<Author> getAuthor() const { return author; }
    std::string getIsbn() const { return isbn; }
    int getYear() const { return year; }
    int getQuantity() const { return quantity; }
    std::string getGenre() const { return genre; }

    void setTitle(const std::string& newTitle) { title = newTitle; }
    void setAuthor(std::shared_ptr<Author> newAuthor) { author = newAuthor; }
    void setIsbn(const std::string& newIsbn) { isbn = newIsbn; }
    void setYear(int newYear) { year = newYear; }
    void setQuantity(int newQuantity) { quantity = newQuantity; }
    void setGenre(const std::string& newGenre) { genre = newGenre; }

    void saveToBinaryFile(std::ofstream& file) const;
    void loadFromBinaryFile(std::ifstream& file, const std::map<std::string, std::shared_ptr<Author>>& authorsMap);
};