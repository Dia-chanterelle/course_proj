#include "../headers/BorrowedBook.h"
#include <fstream>
#include <cstring>

BorrowedBook::BorrowedBook(const std::string& bId, const std::string& rId, const Date& bd)
    : bookId(bId), readerId(rId), borrowDate(bd), returnDate(Date()), returned(false) {
}

void BorrowedBook::saveToBinaryFile(std::ofstream& file) const {
    size_t bookIdSize = bookId.size();
    file.write(reinterpret_cast<const char*>(&bookIdSize), sizeof(bookIdSize));
    file.write(bookId.c_str(), bookIdSize);

    size_t readerIdSize = readerId.size();
    file.write(reinterpret_cast<const char*>(&readerIdSize), sizeof(readerIdSize));
    file.write(readerId.c_str(), readerIdSize);

    int d = borrowDate.getDay(), m = borrowDate.getMonth(), y = borrowDate.getYear();
    file.write(reinterpret_cast<const char*>(&d), sizeof(d));
    file.write(reinterpret_cast<const char*>(&m), sizeof(m));
    file.write(reinterpret_cast<const char*>(&y), sizeof(y));

    int rd = returnDate.getDay(), rm = returnDate.getMonth(), ry = returnDate.getYear();
    file.write(reinterpret_cast<const char*>(&rd), sizeof(rd));
    file.write(reinterpret_cast<const char*>(&rm), sizeof(rm));
    file.write(reinterpret_cast<const char*>(&ry), sizeof(ry));

    file.write(reinterpret_cast<const char*>(&returned), sizeof(returned));
}

void BorrowedBook::loadFromBinaryFile(std::ifstream& file) {
    size_t bookIdSize;
    file.read(reinterpret_cast<char*>(&bookIdSize), sizeof(bookIdSize));
    std::string bId(bookIdSize, '\0');
    file.read(bId.data(), bookIdSize);
    bookId = std::move(bId);

    size_t readerIdSize;
    file.read(reinterpret_cast<char*>(&readerIdSize), sizeof(readerIdSize));
    std::string rId(readerIdSize, '\0');
    file.read(rId.data(), readerIdSize);
    readerId = std::move(rId);

    int d, m, y;
    file.read(reinterpret_cast<char*>(&d), sizeof(d));
    file.read(reinterpret_cast<char*>(&m), sizeof(m));
    file.read(reinterpret_cast<char*>(&y), sizeof(y));
    borrowDate = Date(d, m, y);

    int rd, rm, ry;
    file.read(reinterpret_cast<char*>(&rd), sizeof(rd));
    file.read(reinterpret_cast<char*>(&rm), sizeof(rm));
    file.read(reinterpret_cast<char*>(&ry), sizeof(ry));
    returnDate = Date(rd, rm, ry);

    file.read(reinterpret_cast<char*>(&returned), sizeof(returned));
}

void BorrowedBook::saveAllToBinaryFile(
    const std::map<std::string, BorrowedBook>& borrowedBooksMap,
    const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        size_t count = borrowedBooksMap.size();
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));

        for (const auto& pair : borrowedBooksMap) {
            pair.second.saveToBinaryFile(file);
        }
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