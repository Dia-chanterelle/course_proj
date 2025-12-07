#pragma once
#include "Person.h"
#include "BorrowedBook.h"
#include <vector>
#include <map>

class Reader : public Person {
private:
    std::vector<BorrowedBook> borrowedBooks;

public:
    Reader(const FIO& f = FIO(), const std::string& log = "",
        const std::string& pwd = "", int age = 0,
        const std::string& readerId = "");

    void showMenu() override;
    std::string getRole() const override { return "Reader"; }
    const std::vector<BorrowedBook>& getBorrowedBooks() const { return borrowedBooks; }


    void borrowBook(const std::string& bookId);
    void returnBook(const std::string& bookId);
    void showBorrowedBooks() const;

    void saveToBinaryFile(std::ofstream& file) const override;
    void loadFromBinaryFile(std::ifstream& file) override;

    static void saveAllToBinaryFile(const std::map<std::string, std::shared_ptr<Reader>>& readersMap,
        const std::string& filename);
    static std::map<std::string, std::shared_ptr<Reader>> loadAllFromBinaryFile(const std::string& filename);

protected:
    std::string generateId() const override;
};