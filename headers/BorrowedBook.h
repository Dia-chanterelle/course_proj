#pragma once
#include "Date.h"
#include <string>
#include <vector>
#include <map>


class BorrowedBook {
private:
    std::string bookId;
    std::string readerId;
    Date borrowDate;
    Date returnDate;// срок возврата (borrowDate + 14 дней)
    bool returned;

public:
    BorrowedBook(const std::string& bId = "", const std::string& rId = "",
        const Date& bd = Date());

    std::string getBookId() const { return bookId; }
    std::string getReaderId() const { return readerId; }
    Date getBorrowDate() const { return borrowDate; }
    Date getReturnDate() const { return returnDate; }

    void markReturned();
    bool isReturned() const { return returned; }

    void returnBook(bool);

    void saveToBinaryFile(std::ofstream& file) const;
    void loadFromBinaryFile(std::ifstream& file);


    static void saveAllToBinaryFile(
        const std::map<std::string, BorrowedBook>& borrowedBooksMap,
        const std::string& filename);
    static std::map<std::string, BorrowedBook> loadAllFromBinaryFile(const std::string& filename);
};