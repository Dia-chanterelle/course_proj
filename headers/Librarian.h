#pragma once
#include "Person.h"
#include <vector>
#include <map>

class Librarian : public Person {
private:
    std::string department;

public:
    /*Librarian(const FIO& f = FIO(), const std::string& log = "",
        const std::string& pwd = "", int age = 0,
        const std::string& librarianId = "", const std::string& dept = "Main");*/

    Librarian() : Person(FIO(), "", "", 0, "", true), department("Main") {}

    Librarian(const FIO& f, const std::string& log, const std::string& pwd,
        int age, const std::string& librarianId,
        const std::string& dept, bool isEncrypted = false);

    void showMenu() override;
    std::string getRole() const override { return "Librarian"; }
    const std::string& getDepartment() const { return department; }

    void saveToBinaryFile(std::ofstream& file) const override;
    void loadFromBinaryFile(std::ifstream& file) override;

    static void saveAllToBinaryFile(const std::map<std::string, std::shared_ptr<Librarian>>& librariansMap,
        const std::string& filename);
    static std::map<std::string, std::shared_ptr<Librarian>> loadAllFromBinaryFile(const std::string& filename);

protected:
    //std::string generateId() const override;
};