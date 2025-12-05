#include "../headers/BorrowedBook.h"
#include <fstream>
#include <cstring>

BorrowedBook::BorrowedBook(const std::string& bId, const std::string& rId, const Date& bd)
    : bookId(bId), readerId(rId), borrowDate(bd), returned(false) {
}

void BorrowedBook::saveToBinaryFile(std::ofstream& file) const {
    size_t bookIdSize = bookId.size();
    file.write(reinterpret_cast<const char*>(&bookIdSize), sizeof(bookIdSize));
    file.write(bookId.c_str(), bookIdSize);

    size_t readerIdSize = readerId.size();
    file.write(reinterpret_cast<const char*>(&readerIdSize), sizeof(readerIdSize));
    file.write(readerId.c_str(), readerIdSize);

    int day = borrowDate.getDay();
    int month = borrowDate.getMonth();
    int year = borrowDate.getYear();
    file.write(reinterpret_cast<const char*>(&day), sizeof(day));
    file.write(reinterpret_cast<const char*>(&month), sizeof(month));
    file.write(reinterpret_cast<const char*>(&year), sizeof(year));

    file.write(reinterpret_cast<const char*>(&returned), sizeof(returned));
}

void BorrowedBook::loadFromBinaryFile(std::ifstream& file) {
    size_t bookIdSize;
    file.read(reinterpret_cast<char*>(&bookIdSize), sizeof(bookIdSize));
    char* bookIdBuffer = new char[bookIdSize + 1];
    file.read(bookIdBuffer, bookIdSize);
    bookIdBuffer[bookIdSize] = '\0';
    bookId = bookIdBuffer;
    delete[] bookIdBuffer;

    size_t readerIdSize;
    file.read(reinterpret_cast<char*>(&readerIdSize), sizeof(readerIdSize));
    char* readerIdBuffer = new char[readerIdSize + 1];
    file.read(readerIdBuffer, readerIdSize);
    readerIdBuffer[readerIdSize] = '\0';
    readerId = readerIdBuffer;
    delete[] readerIdBuffer;

    int day, month, year;
    file.read(reinterpret_cast<char*>(&day), sizeof(day));
    file.read(reinterpret_cast<char*>(&month), sizeof(month));
    file.read(reinterpret_cast<char*>(&year), sizeof(year));
    borrowDate = Date(day, month, year);

    file.read(reinterpret_cast<char*>(&returned), sizeof(returned));
}

void BorrowedBook::saveAllToBinaryFile(const std::map<std::string, BorrowedBook>& borrowedBooksMap,
    const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        size_t count = borrowedBooksMap.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const auto& pair : borrowedBooksMap) {
            pair.second.saveToBinaryFile(file);
        }
        file.close();
    }
}

std::map<std::string, BorrowedBook> BorrowedBook::loadAllFromBinaryFile(const std::string& filename) {
    std::map<std::string, BorrowedBook> borrowedBooksMap;
    std::ifstream file(filename, std::ios::binary);

    if (file.is_open()) {
        size_t count;
        file.read(reinterpret_cast<char*>(&count), sizeof(count));

        for (size_t i = 0; i < count; ++i) {
            BorrowedBook book;
            book.loadFromBinaryFile(file);
            std::string key = book.getBookId() + "_" + book.getReaderId();
            borrowedBooksMap[key] = book;
        }
        file.close();
    }

    return borrowedBooksMap;
}